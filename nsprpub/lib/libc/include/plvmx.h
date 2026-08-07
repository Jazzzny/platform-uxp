#include <stddef.h>
#include <string.h>

/* In general, you should include "mozilla-config.h" or the equivalent
   before including this file to choose the proper macro. */

#ifndef _nspr_plvmx_h
#define _nspr_plvmx_h

#if defined(__APPLE__) && (defined(__ppc__) || defined(__POWERPC__) || defined(__powerpc__)) && defined(HAVE_ALTIVEC)

#if defined (__cplusplus)
extern "C" {
#endif

int   PL_vmx_haschr(const void *b, int c, size_t len);
void *PL_vmx_memchr(const void *b, int c, size_t len);
char *PL_vmx_strchr(const char *p, int ch);

#if defined (__cplusplus)
}
#endif

#define VMX_HASCHR PL_vmx_haschr
#define VMX_MEMCHR PL_vmx_memchr
#define VMX_STRCHR PL_vmx_strchr
#else
#if defined (__cplusplus)
#define VMX_HASCHR(a,b,c) (memchr(a,b,c) != nullptr)
#else
#define VMX_HASCHR(a,b,c) (!!memchr(a,b,c))
#endif
#define VMX_MEMCHR memchr
#define VMX_STRCHR strchr
#endif

#endif
