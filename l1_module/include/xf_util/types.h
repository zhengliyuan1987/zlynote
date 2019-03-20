/**
 * @file types.h
 * @brief This file is part of XF Utility Library, contains type definitions.
 */

#ifndef XF_UTIL_TYPES_H
#define XF_UTIL_TYPES_H

// Fixed width integers
#if defined(__cplusplus) && __cplusplus >= 201103L
#include <cstdint>
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <stdint.h>
#else
// Signed.
// avoid conflict with <sys/types.h>
#ifndef __int8_t_defined
#define __int8_t_defined
typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
#if defined(__LP64__) && __LP64__
typedef long int int64_t;
#else
typedef long long int int64_t;
#endif // __LP64__
#endif // __int8_t_defined
// Unsigned.
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
#if defined(__LP64__) && __LP64__
typedef unsigned long int uint64_t;
#else
typedef unsigned long long int uint64_t;
#endif // __LP64__
#endif

#include "hls_stream.h"
#include "ap_int.h"

#endif // ifndef XF_UTIL_TYPES_H

// -*- cpp -*-
// vim: ts=8:sw=2:sts=2:ft=cpp
