#ifndef _3DS_TYPES_FIX_H
#define _3DS_TYPES_FIX_H

// Fix for newlib type definitions that are missing
// These must be defined before including system headers

#ifndef _WINT_T_DEFINED
#define _WINT_T_DEFINED
typedef unsigned int wint_t;
#endif

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#endif

#ifndef __gnuc_va_list_defined
#define __gnuc_va_list_defined
typedef __builtin_va_list __gnuc_va_list;
#endif

#endif // _3DS_TYPES_FIX_H_
