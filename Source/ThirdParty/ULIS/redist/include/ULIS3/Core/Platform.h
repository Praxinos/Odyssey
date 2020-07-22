// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Platform.h
* @author       Clement Berthaud
* @brief        This file provides platform definitions for the ULIS3 library.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include <iostream>

/////////////////////////////////////////////////////
// Detect Build Configuration
#ifdef NDEBUG
    #define ULIS3_RELEASE
#else // !NDEBUG
    #define ULIS3_DEBUG
#endif // !NDEBUG

/////////////////////////////////////////////////////
// Detect Compiler
#if defined(__clang__)
    #define ULIS3_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define ULIS3_GCC
#elif defined(_MSC_VER)
    #define ULIS3_MSVC
#else
    #define ULIS3_UNKNOWN_COMPILER
#endif

/////////////////////////////////////////////////////
// Disable CRT Secure non standard msvc versions of functions such as strcpy_s
#ifdef ULIS3_MSVC
    #define _CRT_SECURE_NO_WARNINGS 1
#endif // ULIS3_MSVC

/////////////////////////////////////////////////////
// Detect Platform
#ifdef __EMSCRIPTEN__
    #define ULIS3_EMSCRIPTEN
#else
    #ifdef _WIN32
        #define ULIS3_WIN
       #ifdef _WIN64
          #define ULIS3_WIN64
       #else
          #define ULIS3_WIN32
       #endif
    #elif __APPLE__
        #include "TargetConditionals.h"
        #if TARGET_IPHONE_SIMULATOR
             #define ULIS3_IOS_SIMULATOR
        #elif TARGET_OS_IPHONE
            #define ULIS3_IOS_DEVICE
        #elif TARGET_OS_MAC
            #define ULIS3_MACOS
        #else
            #error "Unknown Apple platform"
        #endif
    #elif __linux__
        #define ULIS3_LINUX
    #elif __unix__
        #define ULIS3_UNIX
    #elif defined(_POSIX_VERSION)
        #define ULIS3_POSIX
    #else
        #error "Unknown Platform"
    #endif
#endif

/////////////////////////////////////////////////////
// Safety disable thread and SIMD for Emscripten target
#ifdef ULIS3_EMSCRIPTEN
#define ULIS3_NO_THREAD_SUPPORT
#define ULIS3_NO_SIMD_SUPPORT
#endif

/////////////////////////////////////////////////////
// Force Inline Utility
#define ULIS3_ENABLE_FORCEINLINE
#define ULIS3_ENABLE_VECTORCALL

#ifdef ULIS3_ENABLE_FORCEINLINE
    #if defined(__clang__)
    #define ULIS3_FORCEINLINE inline __attribute__ ((always_inline))
    #elif defined(__GNUC__) || defined(__GNUG__)
    #define ULIS3_FORCEINLINE inline __attribute__ ((always_inline))
    #elif defined(_MSC_VER)
    #define ULIS3_FORCEINLINE __forceinline
    #else
    #define ULIS3_FORCEINLINE inline
    #endif
#else
    #define ULIS3_FORCEINLINE inline
#endif // ULIS3_ENABLE_FORCEINLINE

#ifdef ULIS3_ENABLE_VECTORCALL
    #if defined(__clang__)
    #define ULIS3_VECTORCALL __vectorcall
    #elif defined(__GNUC__) || defined(__GNUG__)
    #define ULIS3_VECTORCALL
    #elif defined(_MSC_VER)
    #define ULIS3_VECTORCALL __vectorcall
    #else
    #define ULIS3_VECTORCALL __vectorcall
    #endif
#else
    #define ULIS3_VECTORCALL
#endif // ULIS3_ENABLE_FORCEINLINE

/////////////////////////////////////////////////////
// Export utility macros
#ifdef ULIS3_WIN
    #ifdef ULIS3_BUILD_SHARED
        #define ULIS3_SHARED
        #define ULIS3_API __declspec( dllexport )
        #define ULIS3_API_TEMPLATE template ULIS3_API
    #elif defined ULIS3_DYNAMIC_LIBRARY
        #define ULIS3_SHARED
        #define ULIS3_API __declspec( dllimport )
        #define ULIS3_API_TEMPLATE extern template ULIS3_API
    #else
        #define ULIS3_STATIC
        #define ULIS3_API
        #define ULIS3_API_TEMPLATE template
    #endif
#else
    #define ULIS3_API
    #define ULIS3_API_TEMPLATE template
#endif

/////////////////////////////////////////////////////
// Macros for Thread and SIMD activation, for embeded targets or WASM
#ifndef ULIS3_NO_THREAD_SUPPORT
#define ULIS3_COMPILED_WITH_THREAD_SUPPORT
#endif // !ULIS3_NO_THREAD_SUPPORT

#ifndef ULIS3_NO_SIMD_SUPPORT
#define ULIS3_COMPILED_WITH_SIMD_SUPPORT
#endif // !ULIS3_NO_SIMD_SUPPORT

#ifndef ULIS3_NO_FILESYSTEM_SUPPORT
#define ULIS3_COMPILED_WITH_FILESYSTEM_SUPPORT
#endif // !ULIS3_NO_SIMD_SUPPORT

/////////////////////////////////////////////////////
// Erors
#pragma warning(disable : 4251)     // Shut down dll interface warnings.
#pragma warning(disable : 26812)    // Shut non-class enum warnings.

/////////////////////////////////////////////////////
// Define Namespaces
#define ULIS3_NAMESPACE_NAME        ULIS3
#define ULIS3_SHORT_NAMESPACE_NAME  ul3
#define ULIS3_NAMESPACE_BEGIN       namespace ULIS3_NAMESPACE_NAME {
#define ULIS3_NAMESPACE_END         }
#define ULIS3_FDECL_CLASS( i )      ULIS3_NAMESPACE_BEGIN class i ;     ULIS3_NAMESPACE_END
#define ULIS3_FDECL_STRUCT( i )     ULIS3_NAMESPACE_BEGIN struct i ;    ULIS3_NAMESPACE_END
namespace ULIS3_NAMESPACE_NAME {}
namespace ULIS3_SHORT_NAMESPACE_NAME = ULIS3_NAMESPACE_NAME;

/////////////////////////////////////////////////////
// Version Specification
#define ULIS3_VERSION_MAJOR      3
#define ULIS3_VERSION_MINOR      0
#define ULIS3_VERSION_MAJOR_STR  "3"
#define ULIS3_VERSION_MINOR_STR  "0"

/////////////////////////////////////////////////////
// Crash Behaviours
#define ULIS3_CRASH *((volatile int*)0) = 0

/////////////////////////////////////////////////////
// Assert Behaviours

#if defined( ULIS3_DEBUG ) || defined( ULIS3_FORCE_ASSERT )
    #define ULIS3_ASSERT( cond, log )  if( !( cond ) ) { std::cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " " << "Assertion failed: " << log << std::endl; ULIS3_CRASH; }
#else
    #define ULIS3_ASSERT( cond, log )
#endif

/////////////////////////////////////////////////////
// SIMD PATCH FOR GNU < 9
// CHECK: this conflicts with xcode sometimes identifying itself as GNUC < 9 but defining the appropriate simd instructions.
#ifdef ULIS3_COMPILED_WITH_SIMD_SUPPORT
#if defined( ULIS3_GCC ) && ( defined( ULIS3_WIN ) || defined( ULIS3_LINUX ) )
#if __GNUC__ < 9
// unaligned load and store functions
#define _mm_loadu_si16(p) _mm_cvtsi32_si128(*(unsigned short const*)(p))
#define _mm_storeu_si16(p, a) (void)(*(short*)(p) = (short)_mm_cvtsi128_si32((a)))
#define _mm_loadu_si32(p) _mm_cvtsi32_si128(*(unsigned int const*)(p))
#define _mm_storeu_si32(p, a) (void)(*(int*)(p) = _mm_cvtsi128_si32((a)))
#define _mm_loadu_si64(p) _mm_loadl_epi64((__m128i const*)(p))
#define _mm_storeu_si64(p, a) (_mm_storel_epi64((__m128i*)(p), (a)))
#endif
#endif
#include <immintrin.h>
#endif

/////////////////////////////////////////////////////
// Conditional compile time detection macro in order to decide if we should include SIMD versions in the various dispatch
#ifdef ULIS3_COMPILED_WITH_SIMD_SUPPORT
    #ifdef __AVX2__
        #define ULIS3_COMPILETIME_AVX2_SUPPORT
    #endif
    #ifdef __SSE4_2__
        #define ULIS3_COMPILETIME_SSE42_SUPPORT
    #endif
#endif

/////////////////////////////////////////////////////
// glm FORCE extensions, before any glm related includes
#ifdef ULIS3_COMPILED_WITH_SIMD_SUPPORT
#define GLM_FORCE_SSE42
#define GLM_FORCE_INTRINSICS
#endif
#define GLM_FORCE_SWIZZLE
