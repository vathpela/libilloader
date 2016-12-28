/*
 * libilloader - probably a pretty bad idea
 * Copyright 2016 Peter Jones <pjones@redhat.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#define _GNU_SOURCE 1

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <unistd.h>

#include "libilloader.h"

struct arena {
	struct list_head regions;
	struct list_head sections;
	struct list_head list;
};

struct region {
	void *base;
	ssize_t size;
	int flags;
	int prot;

	bool realized;
	void *map;
	int nr_pages;

	struct list_head list;
};

struct section {
	char *name;
	section_matcher match;

	struct region region;
	struct list_head list;
};

LIST_HEAD(arenae);

static inline ssize_t
base_matcher(const char *self, const char *other, ssize_t n)
{
	int i = -1;
	while (i < n && self && other && self[i] == other[i])
		i++;
	if (i == 1 && self && self[0] == '.')
		return 0;
	return i;
}

static inline int
subsection_matcher(const char *self, const char *other, ssize_t n, int *levelp)
{
	const char *orig = self;
	const char *test = other;
	int level = -1;
	int ret = -1;
	int x = strcspnn(self, ".", -1);

	while (x == 0) {
		self++;
		x = strcspnn(self, ".", -1);
	}

	while (test && (test-other < n) && *test) {
		int y = strcspnn(test, ".", n);
		int m = test - other;

		if (x == 0 || y == 0) {
			if (y == 0)
				test++;
			continue;
		}
		level++;

		if (x == y && x <= n-m && strncmp(self, test, x) == 0) {
			ret = x;
			if (orig[0] == '.')
				ret += 1;
			break;
		}

		test += y;
		if (m && test[0] == '.')
			test++;
	}

	if (ret == -1)
		level = -1;
	if (levelp)
		*levelp = level;
	return ret;
}

#define MAKE_BASE_MATCHER(mname, sname)			\
	static inline ssize_t				\
	mname ## _matcher (const char *name, ssize_t n)	\
	{						\
		return base_matcher(sname, name, n);	\
	}

MAKE_BASE_MATCHER(bss, ".bss");
MAKE_BASE_MATCHER(text, ".text");
MAKE_BASE_MATCHER(data, ".data");

struct {
	char *name;
	section_matcher matcher;
} defaults[] = {
	{.name = ".bss",
	 .matcher = bss_matcher,
	},
	{.name = ".text",
	 .matcher = text_matcher,
	},
	{.name = ".data",
	 .matcher = data_matcher,
	},
	{.name = NULL,
	 .matcher = (section_matcher)NULL,
	}
};

int
llnew_arena(void)
{
	struct arena *arena;

	arena = calloc(1, sizeof(*arena));
	if (!arena)
		return -1;

	INIT_LIST_HEAD(&arena->sections);

	list_add_tail(&arena->list, &arenae);

	return list_entries(&arenae);
}

static inline struct arena *
get_arena_(int arena)
{
	int x = 0;
	struct list_head *pos;

	list_for_each(pos, &arenae) {
		if (++x == arena)
			break;
	}

	if (x == 0) {
		errno = ENOENT;
		return NULL;
	}

	return list_entry(pos, struct arena, list);
}

#define get_arena(a) ({							\
		struct arena *a_ = get_arena_(a);			\
		if (a_ == NULL)						\
		      return -1;					\
		a_;							\
	})

static struct region *
new_region_(void *base, ssize_t size, int flags, int prot)
{
	struct region *region;

	region = calloc(1, sizeof (*region));
	if (!region)
		return NULL;

	region->base = base;
	region->size = size;
	region->flags = flags;
	region->prot = prot;

	region->realized = false;

	return region;
}

#define new_region(a,b,c,d) ({						\
		struct region *r_ = new_region_(a, b, c, d);		\
		if (r_ == NULL)						\
			return -1;					\
		r_;							\
	})

static struct region *
get_region_(struct arena *arena, int rd)
{
	int x = 0;
	struct list_head *pos;

	list_for_each(pos, &arena->regions) {
		if (++x == rd)
			break;
	}

	if (x == 0) {
		errno = ENOENT;
		return NULL;
	}

	return list_entry(pos, struct region, list);
}

#define get_region(a,r) ({						\
		struct arena *a_ = get_arena(a);			\
		struct region *r_ = get_region_(a_, r);			\
		if (r_ == NULL)						\
		      return -1;					\
		r_;							\
	})

int
lladd_region(int ad, void *base, ssize_t size, int flags, int prot)
{
	struct arena *arena;
	struct region *region;

	arena = get_arena(ad);
	region = new_region(base, size, flags, prot);
	list_add_tail(&region->list, &arena->regions);

	return list_entries(&arena->regions);
}

int
llexpand_region(int ad, int rd, ssize_t size_delta)
{
	struct region *region;
	int page_size = sysconf(_SC_PAGESIZE);
	ssize_t new_size;
	int new_nr_pages;

	region = get_region(ad, rd);

	if (add(region->size, size_delta, &new_size)) {
		errno = EOVERFLOW;
		return -1;
	}
	if (new_size <= 0) {
		errno = EINVAL;
		return -1;
	}

	new_nr_pages = new_size / page_size + (new_size % page_size ? 1 : 0);
	if (region->realized) {
		void *new_map;

		new_map = mremap(region->map,
				 page_size * region->nr_pages,
				 page_size * new_nr_pages,
				 MREMAP_MAYMOVE);
		if (!new_map)
			return -1;

		region->map = new_map;

		mprotect(new_map, page_size * new_nr_pages, region->prot);
	}

	region->size = new_size;
	region->nr_pages = new_nr_pages;

	return 0;
}
