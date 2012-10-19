#if _POSIX_C_SOURCE < 200112L && _XOPEN_SOURCE < 600 && !defined(HAVE_POSIX_MEMALIGN)
#include <malloc.h>
#endif

#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#include <stdint.h>
#else
// Non-conforming compilers (especially Microsoft Visual C) need a roll-your-own version.
typedef int bool;
#define true 1
#define false 0
#ifndef _MSC_VER
// This is a definition that MS VC 2005 and later do have.
typedef unsigned long uintptr_t;
#endif // _MSC_VER
#endif // !C99


