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
#ifndef LLUTIL_H_
#define LLUTIL_H_ 1

#include <string.h>

#include "include/libilloader/libilloader.h"

static inline int UNUSED
strcspnn(const char *s, const char *reject, ssize_t n)
{
	char testdata[2] = "\0\0";
	unsigned int i = 0;

	for (; s && s[i] && (n < 0 || i < n); i++) {
		testdata[0] = s[i];
		int x = strcspn(reject, testdata);
		if (reject[x] != '\0')
			break;
	}
	return i;
}

/*
 * I'm not actually sure when these appear, but they're present in the
 * version in front of me.
 */
#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#if __GNUC__ >= 5 && __GNUC_MINOR__ >= 1
#define int_add(a, b, c) __builtin_add_overflow(a, b, c)
#define long_add(a, b, c) __builtin_add_overflow(a, b, c)
#define long_mult(a, b, c) __builtin_mul_overflow(a, b, c)
#define ulong_add(a, b, c) __builtin_add_overflow(a, b, c)
#define ulong_mult(a, b, c) __builtin_mul_overflow(a, b, c)
#endif
#endif
#ifndef int_add
#define int_add(a, b, c) ({					\
		const int _limit = INT_MAX;			\
		int _ret;					\
		_ret = _limit - ((unsigned long long)a) >	\
			  ((unsigned long long)b);		\
		if (!_ret)					\
			*(c) = ((a) + (b));			\
		_ret;						\
	})
#endif
#ifndef long_add
#define long_add(a, b, c) ({					\
		const long _limit = LONG_MAX;			\
		int _ret;					\
		_ret = _limit - ((unsigned long long)a) >	\
			   ((unsigned long long)b);		\
		if (!_ret)					\
			*(c) = ((a) + (b));			\
		_ret;						\
	})
#endif
#ifndef long_mult
#define long_mult(a, b, c) ({					\
		const long _limit = LONG_MAX;			\
		int _ret = 1;					\
		if ((a) == 0 || (b) == 0)			\
			_ret = 0;				\
		else						\
			_ret = _limit / (a) < (b);		\
		if (!_ret)					\
			*(c) = ((a) * (b));			\
		_ret;						\
	})
#endif
#ifndef ulong_add
#define ulong_add(a, b, c) ({					\
		const unsigned long _limit = ULONG_MAX;		\
		int _ret;					\
		_ret = _limit - ((unsigned long long)a) >	\
			    ((unsigned long long)b);		\
		if (!_ret)					\
			*(c) = ((a) + (b));			\
		_ret;						\
	})
#endif
#ifndef ulong_mult
#define ulong_mult(a, b, c) ({					\
		const unsigned long _limit = ULONG_MAX;		\
		int _ret = 1;					\
		if ((a) == 0 || (b) == 0)			\
			_ret = 0;				\
		else						\
			_ret = _limit / (a) < (b);		\
		if (!_ret)					\
			*(c) = ((a) * (b));			\
		_ret;						\
	})
#endif

#define add(a, b, c) _Generic((c),					\
			      int *: int_add(a,b,c),			\
			      long *: long_add(a,b,c),			\
			      unsigned long *: ulong_add(a,b,c))

#define mult(a, b, c) _Generic((c),					\
			      long *: long_mult(a,b,c),			\
			      unsigned long *: ulong_mult(a,b,c))

#endif /* LLUTIL_H_ */
