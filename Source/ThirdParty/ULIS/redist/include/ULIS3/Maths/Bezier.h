// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Bezier.h
* @author       Clement Berthaud
* @brief        This file provides the Bezier methods declaration.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Maths/Geometry.h"
#include "Maths/Maths.h"
#include <vector>

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// Bezier Functions
struct FBezierLUTElement {
    FVec2F point;
    float length;
    float param;
};

struct FBezierCubicControlPoint {
    FVec2F point;
    FVec2F ctrlCW;
    FVec2F ctrlCCW;
};

template< class T > inline T CubicBezierPointAtParameter( const T& iP0, const T& iP1, const T& iP2, const T& iP3, float t ) {
    float u = ( 1 - t );
    float u2 = u*u;
    float t2 = t*t;
    return   iP0 * u2*u
           + iP1 * 3 * u2 * t
           + iP2 * 3 * u * t2
           + iP3 * t2*t;
}

template< class T > inline T QuadraticBezierPointAtParameter( const T& iP0, const T& iP1, const T& iP2, float t ) {
    float u = ( 1 - t );
    return   iP0 * u*u
           + iP1 * 2 * t * u
           + iP2 * t * t;
}

template< class T > inline void QuadraticBezierSplitAtParameter( T* ioP0, T* ioP1, T* ioP2, float t ) {
    *ioP2 = QuadraticBezierPointAtParameter( *ioP0, *ioP1, *ioP2, t );
    *ioP1 = *ioP0 + ( *ioP1 - *ioP0 ) * t;
}

template< class T > inline void QuadraticBezierInverseSplitAtParameter( T* ioP0, T* ioP1, T* ioP2, float t ) {
    *ioP0 = QuadraticBezierPointAtParameter( *ioP0, *ioP1, *ioP2, t );
    *ioP1 = *ioP1 + ( *ioP2 - *ioP1 ) * t;
}

template< class T > inline void CubicBezierSplitAtParameter( T* ioP0, T* ioP1, T* ioP2, T* ioP3, float t ) {
    T A = *ioP0 + ( *ioP1 - *ioP0 ) * t;
    T B = *ioP1 + ( *ioP2 - *ioP1 ) * t;
    T C = *ioP2 + ( *ioP3 - *ioP2 ) * t;
    T D = A + ( B - A ) * t;
    T E = B + ( C - B ) * t;
    T F = D + ( E - D ) * t;
    *ioP3 = F;
    *ioP2 = D;
    *ioP1 = A;
}

template< class T > inline void CubicBezierInverseSplitAtParameter( T* ioP0, T* ioP1, T* ioP2, T* ioP3, float t ) {
    T A = *ioP0 + ( *ioP1 - *ioP0 ) * t;
    T B = *ioP1 + ( *ioP2 - *ioP1 ) * t;
    T C = *ioP2 + ( *ioP3 - *ioP2 ) * t;
    T D = A + ( B - A ) * t;
    T E = B + ( C - B ) * t;
    T F = D + ( E - D ) * t;
    *ioP0 = F;
    *ioP1 = E;
    *ioP2 = C;
}


template< class T > inline FRect QuadraticBezierConvexHullRect( const T& iP0, const T& iP1, const T& iP2 ) {
    int xmin = static_cast< int >( FMaths::RoundToNegativeInfinity( FMaths::Min4( iP0.x, iP1.x, iP2.x ) ) );
    int ymin = static_cast< int >( FMaths::RoundToNegativeInfinity( FMaths::Min4( iP0.y, iP1.y, iP2.y ) ) );
    int xmax = static_cast< int >( FMaths::RoundToPositiveInfinity( FMaths::Max4( iP0.x, iP1.x, iP2.x ) ) );
    int ymax = static_cast< int >( FMaths::RoundToPositiveInfinity( FMaths::Max4( iP0.y, iP1.y, iP2.y ) ) );
    return  FRect::FromMinMax( xmin, ymin, xmax, ymax );
}

template< class T > inline FRect CubicBezierConvexHullRect( const T& iP0, const T& iP1, const T& iP2, const T& iP3 ) {
    int xmin = static_cast< int >( FMaths::RoundToNegativeInfinity( FMaths::Min4( iP0.x, iP1.x, iP2.x, iP3.x ) ) );
    int ymin = static_cast< int >( FMaths::RoundToNegativeInfinity( FMaths::Min4( iP0.y, iP1.y, iP2.y, iP3.y ) ) );
    int xmax = static_cast< int >( FMaths::RoundToPositiveInfinity( FMaths::Max4( iP0.x, iP1.x, iP2.x, iP3.x ) ) );
    int ymax = static_cast< int >( FMaths::RoundToPositiveInfinity( FMaths::Max4( iP0.y, iP1.y, iP2.y, iP3.y ) ) );
    return  FRect::FromMinMax( xmin, ymin, xmax, ymax );
}

ULIS3_API float inline  CubicBezierGenerateLinearLUT_imp( std::vector< FBezierLUTElement >* oArray, const FVec2F& iP0, const FVec2F& iP1, const FVec2F& iP2, const FVec2F& iP3, float iThresholdSquared, float iLengthOffset = 0.f, float iParamOffset = 0.f, float iParamDepth = 1.f ) {
    FVec2F mid = CubicBezierPointAtParameter( iP0, iP1, iP2, iP3, 0.5f );
    float lengthSquaredSegmentA = ( iP0 - mid ).DistanceSquared();
    float lengthSquaredSegmentB = ( mid - iP3 ).DistanceSquared();

    if( lengthSquaredSegmentA >= iThresholdSquared ) {
        FVec2F A = iP0;
        FVec2F B = iP1;
        FVec2F C = iP2;
        FVec2F D = iP3;
        CubicBezierSplitAtParameter( &A, &B, &C, &D, 0.5f );
        lengthSquaredSegmentA = CubicBezierGenerateLinearLUT_imp( oArray, A, B, C, D, iThresholdSquared, iLengthOffset, iParamOffset, iParamDepth * 0.5f );
    }

    oArray->push_back( { mid, ( iLengthOffset + lengthSquaredSegmentA ), iParamOffset + iParamDepth * 0.5f } );

    if( lengthSquaredSegmentB >= iThresholdSquared ) {
        FVec2F A = iP0;
        FVec2F B = iP1;
        FVec2F C = iP2;
        FVec2F D = iP3;
        CubicBezierInverseSplitAtParameter( &A, &B, &C, &D, 0.5f );
        lengthSquaredSegmentB = CubicBezierGenerateLinearLUT_imp( oArray, A, B, C, D, iThresholdSquared, iLengthOffset + lengthSquaredSegmentA, iParamOffset + iParamDepth * 0.5f, iParamDepth * 0.5f );
    }

    return ( lengthSquaredSegmentA + lengthSquaredSegmentB );
}


ULIS3_API float inline CubicBezierGenerateLinearLUT( std::vector< FBezierLUTElement >* oArray, const FVec2F& iP0, const FVec2F& iP1, const FVec2F& iP2, const FVec2F& iP3, float iThreshold ) {
    oArray->clear();
    oArray->push_back( { iP0, 0.f } );
    float length = CubicBezierGenerateLinearLUT_imp( oArray, iP0, iP1, iP2, iP3, iThreshold*iThreshold );
    oArray->push_back( { iP3, length, 1.f } );
    return  length;
}

/*
void inline CubicBezierRelinearizeLUT( const std::vector< FBezierLUTElement >& iArray, std::vector< FBezierLUTElement >* oArray, float iThreshold ) {
    oArray->clear();
    oArray->reserve( static_cast< size_t >( iArray.size() * iThreshold ) );
    for( int i = 1; i < iArray.size(); ++i ) {
        const FBezierLUTElement& prev = iArray[i-1];
        const FBezierLUTElement& curr = iArray[i];
        const FVec2F& prevP = prev.point;
        const FVec2F& currP = curr.point;
        FVec2F delta = currP - prevP;
        const float scale = FMaths::Max( delta.x, delta.y ); // Scale so that bigger dim steps 1
        const int scalei = static_cast< int >( scale );
        delta /= scale;
        float dtdist = ( curr.length - prev.length ) / scale;
        FVec2F pt = prevP;
        for( int i = 0; i < scalei; ++i ) {
            pt += delta;
            oArray->push_back( { pt, prev.length + dtdist * i } );
        }
    }
    oArray->push_back( iArray.back() );
}
*/

ULIS3_NAMESPACE_END

