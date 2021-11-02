// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         CatmullRom.h
* @author       Clement Berthaud
* @brief        This file provides the CatmullRom methods implementation.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/Rectangle.h"
#include "Math/Geometry/Vector.h"
#include "Math/Interpolation/Spline.h"
#include "Math/Math.h"
#include <cmath>
#include <vector>

ULIS_NAMESPACE_BEGIN
static constexpr float sCatmullRomKnotParametricConstant_Uniform        = 0.0f;
static constexpr float sCatmullRomKnotParametricConstant_Centripetal    = 0.5f;
static constexpr float sCatmullRomKnotParametricConstant_Chordal        = 1.f;

template< class T >
float NextKnot(float t, const T& p0, const T& p1, float alpha ) {
    float a = pow((p1.x-p0.x), 2.0f) + pow((p1.y-p0.y), 2.0f);
    float b = pow(a, alpha * 0.5f);
    return (b + t);
}

template< class T >
void
CatmullRomPoints( const T& iP0
                , const T& iP1
                , const T& iP2
                , const T& iP3
                , uint32 iNumPoints
                , std::vector< T >* oOut
                , float iKnotParameter = sCatmullRomKnotParametricConstant_Centripetal )
{
    oOut->clear();

    // TODO: using epsilon and shifting inputs slightly makes it easy to handle the special case when two successive control points are equal
    // in the definition of a cubic hermite spline, the point is to extract tangent from the derivative at each control point.
    // CatmullRom is just a name for a particular way to chose tangents, and you cannot extract tangent from two points with same value
    // since their delta is 0. By examining the calculations you can see it can result in division by zero.
    // So we shift inputs in such a way no two successive control points Pi are equals.
    // A more elegant way to express the undefined tangent is to chose the alignment of the next point and align towards that, instead of
    // a random epsilon value. This will yield better looking results and does not make assumptions on an arbitrary orientation,
    // at the cost of a normalization and a few other operations.
    static constexpr float epsilon = 10E-3f;
    T P0 = iP0;
    T P1 = P0 == iP1 ? iP1 + FVec2F( epsilon, 0.0f ) : iP1;
    T P2 = P1 == iP2 ? iP2 + FVec2F( 0.0f, epsilon ) : iP2;
    T P3 = P2 == iP3 ? iP3 - FVec2F( epsilon, 0.0f ) : iP3;

    float t0 = 0.0f;
    float t1 = NextKnot( t0, P0, P1, iKnotParameter );
    float t2 = NextKnot( t1, P1, P2, iKnotParameter );
    float t3 = NextKnot( t2, P2, P3, iKnotParameter );
    float t1mt0 = ( t1 - t0 );
    float t2mt1 = ( t2 - t1 );
    float t3mt2 = ( t3 - t2 );
    float t2mt0 = ( t2 - t0 );
    float t3mt1 = ( t3 - t1 );

    for( float t = t1; t < t2; t += ( ( t2 - t1 ) / (float)iNumPoints ) ) {
        T A1 = P0 * ( t1 - t ) / t1mt0 + P1 * ( t - t0 ) / t1mt0;
        T A2 = P1 * ( t2 - t ) / t2mt1 + P2 * ( t - t1 ) / t2mt1;
        T A3 = P2 * ( t3 - t ) / t3mt2 + P3 * ( t - t2 ) / t3mt2;
        T B1 = A1 * ( t2 - t ) / t2mt0 + A2 * ( t - t0 ) / t2mt0;
        T B2 = A2 * ( t3 - t ) / t3mt1 + A3 * ( t - t1 ) / t3mt1;
        T C  = B1 * ( t2 - t ) / t2mt1 + B2 * ( t - t1 ) / t2mt1;
        oOut->push_back( C );
    }
}

struct FCatmullRomSpline {
public:
    FCatmullRomSpline( const FVec2F& iP0
                     , const FVec2F& iP1
                     , const FVec2F& iP2
                     , const FVec2F& iP3
                     , float iAlpha = sCatmullRomKnotParametricConstant_Centripetal ) {
        static constexpr float epsilon = 10E-3f;
        P0 = iP0;
        P1 = P0 == iP1 ? iP1 + FVec2F( epsilon, 0.0f ) : iP1;
        P2 = P1 == iP2 ? iP2 + FVec2F( 0.0f, epsilon ) : iP2;
        P3 = P2 == iP3 ? iP3 - FVec2F( epsilon, 0.0f ) : iP3;

        t0 = 0.0f;
        t1 = NextKnot( t0, P0, P1, iAlpha );
        t2 = NextKnot( t1, P1, P2, iAlpha );
        t3 = NextKnot( t2, P2, P3, iAlpha );
        t1mt0 = ( t1 - t0 );
        t2mt1 = ( t2 - t1 );
        t3mt2 = ( t3 - t2 );
        t2mt0 = ( t2 - t0 );
        t3mt1 = ( t3 - t1 );
        alpha = iAlpha;
    }

public:
    FVec2F Eval( float iT ) {
        float t = t1 + t2mt1 * iT;
        FVec2F A1 = P0 * ( t1 - t ) / t1mt0 + P1 * ( t - t0 ) / t1mt0;
        FVec2F A2 = P1 * ( t2 - t ) / t2mt1 + P2 * ( t - t1 ) / t2mt1;
        FVec2F A3 = P2 * ( t3 - t ) / t3mt2 + P3 * ( t - t2 ) / t3mt2;
        FVec2F B1 = A1 * ( t2 - t ) / t2mt0 + A2 * ( t - t0 ) / t2mt0;
        FVec2F B2 = A2 * ( t3 - t ) / t3mt1 + A3 * ( t - t1 ) / t3mt1;
        FVec2F C  = B1 * ( t2 - t ) / t2mt1 + B2 * ( t - t1 ) / t2mt1;
        return  C;
    }

    void GenerateLinearLUT( std::vector< FSplineLinearSample >* oArray, float iStep ) {
        oArray->clear();
        oArray->push_back( { P1, 0.f } );
        float length = GenerateLinearLUTAndGetLength_imp( oArray, iStep, P1, P2 );
        oArray->push_back( { P2, length } );
    }

private:
    float GenerateLinearLUTAndGetLength_imp( std::vector< FSplineLinearSample >* oArray
                                           , float iStep
                                           , const FVec2F& iLeft
                                           , const FVec2F& iRight
                                           , float iLengthOffset = 0.f
                                           , float iParamOffset = 0.f
                                           , float iParamDepth = 1.f ) {
        FVec2F mid = Eval( iParamOffset + iParamDepth * 0.5f );
        float lengthSegmentA = ( iLeft - mid  ).Distance();
        float lengthSegmentB = ( mid - iRight ).Distance();

        if( lengthSegmentA >= iStep ) {
            lengthSegmentA = GenerateLinearLUTAndGetLength_imp( oArray, iStep, iLeft, mid, iLengthOffset, iParamOffset, iParamDepth * 0.5f );
        }

        oArray->push_back( { mid, ( iLengthOffset + lengthSegmentA ) } );

        if( lengthSegmentB >= iStep ) {
            lengthSegmentB = GenerateLinearLUTAndGetLength_imp( oArray, iStep, mid, iRight, iLengthOffset + lengthSegmentA, iParamOffset + iParamDepth * 0.5f, iParamDepth * 0.5f );
        }

        return ( lengthSegmentA + lengthSegmentB );
    }

private:
    FVec2F P0;
    FVec2F P1;
    FVec2F P2;
    FVec2F P3;
    float t0;
    float t1;
    float t2;
    float t3;
    float t1mt0;
    float t2mt1;
    float t3mt2;
    float t2mt0;
    float t3mt1;
    float alpha;
};

ULIS_NAMESPACE_END

