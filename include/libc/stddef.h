#ifndef STDDEF_H
#define STDDEF_H

#include <3ds/types.h>

#define size_t u32
#define ptrdiff_t s32

#ifndef offsetof
#define offsetof(st, m) ((u32)&(((st *)0)->m))
#endif

#define NULL ((void*)0)

#endif
