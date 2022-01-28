// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Morton.h
* @author       Clement Berthaud
* @brief        This file provides the macros for the Morton utilities.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TMortonEncodeKeys8bit
/// @brief      The TMortonEncodeKeys8bit class provides a mean of generating compile-time
///             LUTs for mortons keys, used in linear quad trees.
/// @details    Limitations: it is meant as a quick automated tool for 2D, not 3D
///             codes, and cannot exceed 8 bit interleaved codes, so range is
///             limited in both x & y to [0;16[, which makes it suitable for
///             1024 Qtrees at most.
template< uint8 N, uint8 O >
struct TMortonEncodeKeys8bit
{
    constexpr TMortonEncodeKeys8bit< N, O >()
        : keys{ 0 }
    {
        static_assert( N > 0, "" );
        static_assert( N <= 16, "" );
        for( uint8 x = 0; x < N; ++x )
            for( uint8 i = 0; i < 8; ++i )
                keys[x] |= ( x & ( 1 << i ) ) << ( i + O );
    }

    uint8 keys[N];
};

/////////////////////////////////////////////////////
/// @class      TMortonDecodeKeys8bit2D
/// @brief      The TMortonDecodeKeys8bit2D class provides a mean of generating compile-time
///             LUTs for revertse mortons keys, used in linear quad trees.
/// @details    Limitations: it is meant as a quick automated tool for 2D, not 3D
///             codes, and cannot exceed 8 bit interleaved codes, so range is
///             limited in both x & y to [0;16[, which makes it suitable for
///             1024 Qtrees at most.
template< uint16 N >
struct TMortonDecodeKeys8bit2D
{
    struct FDecodedPoint {
        uint8 x, y;
    };

    constexpr TMortonDecodeKeys8bit2D< N >()
        : keys{ { 0, 0 } }
    {
        static_assert( N > 0, "" );
        static_assert( N <= 256, "" );
        for( uint16 i = 0; i < N; ++i ) {
            for( uint8 b = 0; b < 4; ++b ) {
                keys[i].x |= ( i & ( 1 << b * 2 ) ) >> b;
                keys[i].y |= ( ( i >> 1 ) & ( 1 << b * 2 ) ) >> b;
            }
        }
    }

    FDecodedPoint keys[N];
};

/////////////////////////////////////////////////////
/// @class      FMortonFunctionKeys8bit2D
/// @brief      The FMortonFunctionKeys8bit2D class provides a mean of
///             doing small arithmetic operations on 2D 8bit morton keys
/// @details    Limitations: it is meant as a quick automated tool for 2D, not 3D
///             codes, and cannot exceed 8 bit interleaved codes, so range is
///             limited in both x & y to [0;16[, which makes it suitable for
///             1024 Qtrees at most.
struct ULIS_API FMortonFunctionKeys8bit2D
{
    /*! Incrementation on X, no saturation check, may overflow */
    static uint8 IncX( uint8 iKey = 1 );

    /*! Incrementation on Y, no saturation check, may overflow */
    static uint8 IncY( uint8 iKey = 1 );
};

ULIS_NAMESPACE_END

