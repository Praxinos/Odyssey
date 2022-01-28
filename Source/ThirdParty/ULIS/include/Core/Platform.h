// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Platform.h
* @author       Clement Berthaud
* @brief        This file provides platform definitions for the ULIS library.
* @license      Please refer to LICENSE.md
*/
#pragma once
/////////////////////////////////////////////////////
// Disable CRT Secure non standard msvc versions of functions such as strcpy_s
#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

/////////////////////////////////////////////////////
// For ASSERT logs
#include <iostream>

/////////////////////////////////////////////////////
// STRINGIFY
#define ULIS_XSTRINGIFY( X ) #X
#define ULIS_STRINGIFY( X ) ULIS_XSTRINGIFY( X )

/////////////////////////////////////////////////////
// Detect Build Configuration
#ifndef ULIS_RELWITHDEBINFO
    #ifdef NDEBUG
        #define ULIS_RELEASE
    #else // !NDEBUG
        #define ULIS_DEBUG
    #endif // !NDEBUG
#endif

/////////////////////////////////////////////////////
// Detect Compiler
#if defined(__clang__)
    #define ULIS_CLANG
#elif defined(_MSC_VER)
    #define ULIS_MSVC
#elif defined(__MINGW64__)
    #define ULIS_MINGW64
    #define ULIS_MINGW
#elif defined(__MINGW32__)
    #define ULIS_MINGW32
    #define ULIS_MINGW
#elif defined(__GNUC__) || defined(__GNUG__)
    #define ULIS_GCC
#else
    #define ULIS_UNKNOWN_COMPILER
#endif

/////////////////////////////////////////////////////
// Detect Platform
#ifdef __EMSCRIPTEN__
    #define ULIS_EMSCRIPTEN
#else
    #ifdef _WIN32
        #define ULIS_WIN
       #ifdef _WIN64
          #define ULIS_WIN64
       #else
          #define ULIS_WIN32
       #endif
    #elif __APPLE__
        #include "TargetConditionals.h"
        #if TARGET_IPHONE_SIMULATOR
             #define ULIS_IOS_SIMULATOR
        #elif TARGET_OS_IPHONE
            #define ULIS_IOS_DEVICE
        #elif TARGET_OS_MAC
            #define ULIS_MACOS
        #else
            #error "Unknown Apple platform"
        #endif
    #elif __linux__
        #define ULIS_LINUX
    #elif __unix__
        #define ULIS_UNIX
    #elif defined(_POSIX_VERSION)
        #define ULIS_POSIX
    #else
        #error "Unknown Platform"
    #endif
#endif

/////////////////////////////////////////////////////
// Disable thread and SIMD for Emscripten target
#ifdef ULIS_EMSCRIPTEN
#ifndef __EMSCRIPTEN_PTHREADS__
#define ULIS_NO_THREAD_SUPPORT
#endif
#define ULIS_NO_SIMD_SUPPORT
#endif

/////////////////////////////////////////////////////
// Force Inline Utility
#define ULIS_ENABLE_FORCEINLINE
#define ULIS_ENABLE_VECTORCALL
#define ULIS_ENABLE_RESTRICT

#ifdef ULIS_ENABLE_FORCEINLINE
    #if defined(__clang__)
    #define ULIS_FORCEINLINE inline __attribute__ ((always_inline))
    #elif defined(__GNUC__) || defined(__GNUG__)
    #define ULIS_FORCEINLINE inline __attribute__ ((always_inline))
    #elif defined(_MSC_VER)
    #define ULIS_FORCEINLINE __forceinline
    #else
    #define ULIS_FORCEINLINE inline
    #endif
#else
    #define ULIS_FORCEINLINE inline
#endif // ULIS_ENABLE_FORCEINLINE

#ifdef ULIS_ENABLE_VECTORCALL
    #if defined(__clang__)
    #define ULIS_VECTORCALL __vectorcall
    #elif defined(__GNUC__) || defined(__GNUG__)
    #define ULIS_VECTORCALL
    #elif defined(_MSC_VER)
    #define ULIS_VECTORCALL __vectorcall
    #else
    #define ULIS_VECTORCALL __vectorcall
    #endif
#else
    #define ULIS_VECTORCALL
#endif // ULIS_ENABLE_FORCEINLINE

#ifdef ULIS_ENABLE_RESTRICT
    #if defined(__clang__)
    #define ULIS_RESTRICT __restrict
    #elif defined(__GNUC__) || defined(__GNUG__)
    #define ULIS_RESTRICT __restrict
    #elif defined(_MSC_VER)
    #define ULIS_RESTRICT __restrict
    #else
    #define ULIS_RESTRICT __restrict
    #endif
#else
    #define ULIS_RESTRICT
#endif // ULIS_ENABLE_RESTRICT

/////////////////////////////////////////////////////
// Export utility macros
#ifdef ULIS_WIN
    #ifdef ULIS_STATIC_LIBRARY
        #define ULIS_BUILT_AS_STATIC_LIBRARY
        #define ULIS_API
    #else
        #ifdef ULIS_BUILD_SHARED
            #define ULIS_BUILT_AS_SHARED_LIBRARY
            #define ULIS_API __declspec( dllexport )
        #else
            #define ULIS_BUILT_AS_SHARED_LIBRARY
            #define ULIS_API __declspec( dllimport )
        #endif
    #endif
#else
    #define ULIS_API
#endif

/////////////////////////////////////////////////////
// Macros for Thread and SIMD activation, for embeded targets or WASM
#ifndef ULIS_NO_THREAD_SUPPORT
#define ULIS_COMPILED_WITH_THREAD_SUPPORT
#endif // !ULIS_NO_THREAD_SUPPORT

#ifndef ULIS_NO_SIMD_SUPPORT
#define ULIS_COMPILED_WITH_SIMD_SUPPORT
#endif // !ULIS_NO_SIMD_SUPPORT

#ifndef ULIS_NO_FILESYSTEM_SUPPORT
#define ULIS_COMPILED_WITH_FILESYSTEM_SUPPORT
#endif // !ULIS_NO_SIMD_SUPPORT

/////////////////////////////////////////////////////
// Macros for features support
#define ULIS_FEATURE_NO_GPU
#ifndef ULIS_FEATURE_NO_GPU
#define ULIS_FEATURE_GPU_ENABLED
#endif // !ULIS_FEATURE_NO_GPU

#ifndef ULIS_FEATURE_NO_BLEND
#define ULIS_FEATURE_BLEND_ENABLED
#endif // !ULIS_FEATURE_NO_BLEND

#ifndef ULIS_FEATURE_NO_CLEAR
#define ULIS_FEATURE_CLEAR_ENABLED
#endif // !ULIS_FEATURE_NO_CLEAR

#ifndef ULIS_FEATURE_NO_CONV
#define ULIS_FEATURE_CONV_ENABLED
#endif // !ULIS_FEATURE_NO_CONV

#ifndef ULIS_FEATURE_NO_COPY
#define ULIS_FEATURE_COPY_ENABLED
#endif // !ULIS_FEATURE_NO_COPY

#ifndef ULIS_FEATURE_NO_FILL
#define ULIS_FEATURE_FILL_ENABLED
#endif // !ULIS_FEATURE_NO_FILL

#ifndef ULIS_FEATURE_NO_TEXT
#define ULIS_FEATURE_TEXT_ENABLED
#endif // !ULIS_FEATURE_NO_TEXT

#ifndef ULIS_FEATURE_NO_TRANSFORM
#define ULIS_FEATURE_TRANSFORM_ENABLED
#endif // !ULIS_FEATURE_NO_TRANSFORM

#ifndef ULIS_FEATURE_NO_IO
#define ULIS_FEATURE_IO_ENABLED
#endif // !ULIS_FEATURE_NO_IO

/////////////////////////////////////////////////////
// Erors
#pragma warning(disable : 4251)     // Shut down dll interface warnings.
#pragma warning(disable : 26812)    // Shut non-class enum warnings.
#pragma warning(disable : 4010)     // Shut single-line comment contains line-continuation character
#pragma warning(disable : 4996)     // Shut CRT SECURE
#pragma warning(disable : 4250)     // Virtual inheritance by dominance
/////////////////////////////////////////////////////
// Define Namespaces
#define ULIS_NAMESPACE_NAME        ULIS
//#define ULIS_SHORT_NAMESPACE_NAME  ul
#define NEW_JOBSYSTEM
#define ULIS_NAMESPACE_BEGIN       namespace ULIS_NAMESPACE_NAME {
#define ULIS_NAMESPACE_END         }
namespace ULIS_NAMESPACE_NAME {}
//namespace ULIS_SHORT_NAMESPACE_NAME = ULIS_NAMESPACE_NAME;

/////////////////////////////////////////////////////
// Crash Behaviours
#define ULIS_CRASH *((volatile int*)0) = 0

/////////////////////////////////////////////////////
// Assert Behaviours

#define ULIS_WARNING( cond, log )

#if ( !defined( ULIS_EMSCRIPTEN ) ) && ( defined( ULIS_DEBUG ) || defined( ULIS_RELWITHDEBINFO ) || defined( ULIS_FORCE_ASSERT ) )
    #define ULIS_ASSERT( cond, log )  if( !( cond ) ) { std::cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " " << "Assertion failed: " << log << std::endl; ULIS_CRASH; }
    #define ULIS_ASSERT_RETURN_ERROR( cond, log, ret )  if( !( cond ) ) { std::cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " " << "Assertion failed: " << log << std::endl; ULIS_CRASH; return  ret; }
    #define ULIS_ASSERT_ENABLED
#else
    #define ULIS_ASSERT( cond, log )
    #define ULIS_ASSERT_RETURN_ERROR( cond, log, ret ) if( !( cond ) ) { return  ret; }
#endif

#define ULIS_ENABLE_DEBUG_PRINTF
#ifdef ULIS_ENABLE_DEBUG_PRINTF
    #define ULIS_DEBUG_PRINTF( log ) { std::cout << log << std::endl; }
#else
    #define ULIS_DEBUG_PRINTF( log )
#endif

/////////////////////////////////////////////////////
// SIMD PATCH FOR GNU < 9
// CHECK: this conflicts with xcode sometimes identifying itself as GNUC < 9 but defining the appropriate simd instructions.
#ifdef ULIS_COMPILED_WITH_SIMD_SUPPORT
#if ( defined( ULIS_GCC ) || defined( ULIS_MINGW ) ) && ( defined( ULIS_WIN ) || defined( ULIS_LINUX ) )
#if __GNUC__ < 9
// unaligned load and store functions
#define _mm_loadu_si16(p) _mm_cvtsi32_si128(*(unsigned short const*)(p))
#define _mm_storeu_si16(p, a) (void)(*(short*)(p) = (short)_mm_cvtsi128_si32((a)))
#define _mm_loadu_si32(p) _mm_cvtsi32_si128(*(unsigned int const*)(p))
#define _mm_storeu_si32(p, a) (void)(*(int*)(p) = _mm_cvtsi128_si32((a)))
#define _mm_loadu_si64(p) _mm_loadl_epi64((__m128i const*)(p))
#define _mm_storeu_si64(p, a) (_mm_storel_epi64((__m128i*)(p), (a)))
#else // GNUC 10+
//#define _mm_loadu_si32(p) _mm_castps_si128( _mm_load_ss((float*)p) )
//#define _mm_storeu_si32(p, a) _mm_store_ss((float*)p, _mm_castsi128_ps(a))
#define _mm_loadu_si32(p) _mm_cvtsi32_si128(*(unsigned int const*)(p))
#define _mm_storeu_si32(p, a) (void)(*(int*)(p) = _mm_cvtsi128_si32((a)))
#endif
#endif
#include <immintrin.h>
#endif

/////////////////////////////////////////////////////
// Conditional compile time detection macro in order to decide if we should include SIMD versions in the various dispatch
#ifdef ULIS_COMPILED_WITH_SIMD_SUPPORT
    #ifdef __AVX2__
        #define ULIS_COMPILETIME_AVX_SUPPORT
    #endif
    #ifdef __SSE4_2__
        #define ULIS_COMPILETIME_SSE_SUPPORT
    #endif
#endif

/////////////////////////////////////////////////////
// Conditional Debug Statistics
#if defined( ULIS_DEBUG ) || defined( ULIS_RELWITHDEBINFO ) || defined( ULIS_FORCE_STATISTICS )
    #define ULIS_STATISTICS_ENABLED
#endif

/////////////////////////////////////////////////////
// Disable REGISTER Keyword for LCMS2
#define CMS_NO_REGISTER_KEYWORD

/////////////////////////////////////////////////////
// Define BL_STATIC for use of blend2d compiled as static library
// Not necessary per say since it is inherited from blend2d target
#define BL_STATIC 1

/////////////////////////////////////////////////////
// For python binding, fake factory create
#define ULIS_FAKE_FACTORY_NAME __do_not_call__
#define ULIS_DECLARE_FAKE_FACTORY( iType ) static iType __do_not_call__();
#define ULIS_DEFINE_FAKE_FACTORY( iType, ... ) iType iType :: __do_not_call__() { return  iType ( __VA_ARGS__ ); }

