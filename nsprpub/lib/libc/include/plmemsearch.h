#ifndef _nspr_plmemsearch_h
#define _nspr_plmemsearch_h

#include <string.h>

#if defined(__APPLE__) && (defined(__ppc__) || defined(__POWERPC__) || defined(__powerpc__)) && defined(HAVE_ALTIVEC)
#include "plvmx.h"
#define PL_HASCHR VMX_HASCHR
#define PL_MEMCHR VMX_MEMCHR
#define PL_STRCHR VMX_STRCHR
#else
#if defined(__cplusplus)
#define PL_HASCHR(a, b, c) (memchr(a, b, c) != nullptr)
#else
#define PL_HASCHR(a, b, c) (!!memchr(a, b, c))
#endif
#define PL_MEMCHR memchr
#define PL_STRCHR strchr
#endif

#endif
