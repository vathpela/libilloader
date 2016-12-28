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

#include <err.h>
#include <stdio.h>

#include "../libilloader.h"
#include "../arena.c"

struct {
	char *left;
	char *right;
	size_t limit;
	int expected;
} strcspnn_tests[] = {
	{"abcdef", "vwxyzf", 5, 5},
	{"abcdef", "vwxyzf", 6, 5},
	{"abcdef", "uvwxyz", 20, 6},
	{NULL, "abc", 0, 0},
	{NULL, NULL, 0, 0}
};

struct {
	char *other;
	ssize_t limit;
	int expected;
} base_matcher_tests[] = {
	{".bss", 4, 4},
	{".bss", 3, 3},
	{".bss.subsection", 15, 4},
	{".bss.subsection", 16, 4},
	{".data", 5, 0},
	{".data", 6, 0},
	{".data.bss", 9, 0},
	{".data.bss", 10, 0},
	{".", 1, 0},
	{NULL, -1, 0}
};

struct {
	char *name;
	ssize_t limit;
	int expected;
	int expected_level;
} sub_matcher_tests[] = {
	{".data.vm.0", 10, 3, 1 },
	{".text.vm.0", 10, 3, 1 },
	{".vm.data.0", 10, 3, 0 },
	{".data", 5, -1, -1, },
	{NULL, -1, 0 ,0 }
};

int
main(void)
{
	for (int i = 0; strcspnn_tests[i].right != NULL; i++) {
		int x = strcspnn(strcspnn_tests[i].left,
				 strcspnn_tests[i].right,
				 strcspnn_tests[i].limit);
		if (x != strcspnn_tests[i].expected)
			errx(1, "strcspnn(\"%s\", \"%s\", %zd) == %d, expected %d\n",
			     strcspnn_tests[i].left,
			     strcspnn_tests[i].right,
			     strcspnn_tests[i].limit,
			     x,
			     strcspnn_tests[i].expected);
	}

	for (int i = 0; base_matcher_tests[i].other != NULL; i++) {
		int x = bss_matcher(base_matcher_tests[i].other,
				    base_matcher_tests[i].limit);
		if (x != base_matcher_tests[i].expected)
			errx(2, "bss_matcher(\"%s\", %zd) = %d, expected %d\n",
			     base_matcher_tests[i].other,
			     base_matcher_tests[i].limit,
			     x,
			     base_matcher_tests[i].expected);
	}

	for (int i = 0; sub_matcher_tests[i].name != NULL; i++) {
		int level = 0;
		int x = subsection_matcher(".vm",
					   sub_matcher_tests[i].name,
					   sub_matcher_tests[i].limit,
					   &level);
		if (x != sub_matcher_tests[i].expected ||
		    level != sub_matcher_tests[i].expected_level)
			errx(3, "subsection_matcher(\".vm\", \"%s\", %zd, level=%d) = %d, expected level=%d, %d\n",
			     sub_matcher_tests[i].name,
			     sub_matcher_tests[i].limit,
			     level,
			     x,
			     sub_matcher_tests[i].expected_level,
			     sub_matcher_tests[i].expected);
	}


	llnew_arena();

	int x = 0;
	struct list_head *pos;
	list_for_each(pos, &arenae)
		x++;

	if (x != 1)
		errx(4, "list should have 1 entry, has %d\n", x);

	return 0;
}
