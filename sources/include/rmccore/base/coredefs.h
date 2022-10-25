//
//  coredefs.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_BASE_COREDEFS_H__
#define __RMCCORE_BASE_COREDEFS_H__

#include <cstdint>
#ifndef __cplusplus
#include <cstdbool>
#endif

typedef void *				RMSHANDLE;
#if !defined(UNREFERENCED_PARAMETER)
#define UNREFERENCED_PARAMETER(P)          (P)
#endif

#ifndef _WIN32
//    typedef int32_t         BOOL;
    typedef int32_t         INT, INT32, LONG;
    typedef int64_t         INT64, LONGLONG;
    typedef uint32_t        UINT, UINT32, ULONG, DWORD, *PDWORD;
    typedef uint64_t        UINT64, ULONGLONG, QWORD, DWORD64, *PDWORD64;
    typedef void            VOID, *PVOID, *HANDLE;
#   if defined(__AMD64__) || defined(__X64__) || defined(__x64__) || defined(__APPLE__)
        typedef uint64_t    ULONG_PTR;
#   else
        typedef uint32_t    ULONG_PTR;
#   endif
#endif  /*!defined(_WIN32)*/

#ifdef _MSC_VER
#   ifndef FORCEINLINE
#       define FORCEINLINE  __forceinline
#   endif
#else
#   define FORCEINLINE  inline __attribute__((always_inline))
#endif

#endif /* __RMCCORE_BASE_COREDEFS_H__ */
