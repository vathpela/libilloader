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
#ifndef LIBILLOADER_H
#define LIBILLOADER_H 1

#include "config.h"

extern int llnew_arena(size_t size) EXPORT;
extern int lladd_arena(void *base, size_t size) EXPORT;

extern void *llopen(const char *filename, int flags) EXPORT;

#endif /* LIBILLOADER_H */
