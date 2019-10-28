// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct  FOdysseyMathUtils
{
    struct  FOdysseyBezierLutElement
    {
        FVector2D point;
        float length;
    };

    template< class T >
    static
    T
    CubicBezierPointAtParameter( const  T&  iP0,
                                 const  T&  iP1,
                                 const  T&  iP2,
                                 const  T&  iP3,
                                 float  t )
    {
        return  iP0 * ( 1 - t ) * ( 1 - t ) * ( 1 - t ) +
                3 * iP1 * t * ( 1 - t ) * ( 1 - t ) +
                3 * iP2 * t * t * ( 1 - t ) +
                iP3 * t * t * t;
    }

    template< class T >
    static
    T
    QuadraticBezierPointAtParameter( const  T&  iP0,
                                     const  T&  iP1,
                                     const  T&  iP2,
                                     float  t )
    {
        return  iP0 * ( 1 - t ) * ( 1 - t ) +
                2 * iP1 * t * ( 1 - t ) +
                iP2 * t * t;
    }


    static
    void
    QuadraticBezierSplitAtParameter( FVector2D*  ioP0,
                                     FVector2D*  ioP1,
                                     FVector2D*  ioP2,
                                     float  t )
    {
        *ioP2 = QuadraticBezierPointAtParameter( *ioP0, *ioP1, *ioP2, t );
        *ioP1 = *ioP0 + ( *ioP1 - *ioP0 ) * t;
    }

    static
    void
    QuadraticBezierInverseSplitAtParameter( FVector2D*  ioP0,
                                            FVector2D*  ioP1,
                                            FVector2D*  ioP2,
                                            float  t )
    {
        *ioP0 = QuadraticBezierPointAtParameter( *ioP0, *ioP1, *ioP2, t );
        *ioP1 = *ioP1 + ( *ioP2 - *ioP1 ) * t;
    }

    static
    void
    QuadraticBezierSplitMiddle( FVector2D*  ioP0,
                                FVector2D*  ioP1,
                                FVector2D*  ioP2 )
    {
        *ioP2 = QuadraticBezierMiddlePoint( *ioP0, *ioP1, *ioP2 );
        *ioP1 = ( *ioP0 + *ioP1 ) / 2.f;
    }

    static
    void
    QuadraticBezierInverseSplitMiddle( FVector2D*  ioP0,
                                       FVector2D*  ioP1,
                                       FVector2D*  ioP2 )
    {
        *ioP0 = QuadraticBezierMiddlePoint( *ioP0, *ioP1, *ioP2 );
        *ioP1 = ( *ioP1 + *ioP2 ) / 2.f;
    }

    template< class T >
    static
    T
    QuadraticBezierMiddlePoint( const  T&  iP0,
                                const  T&  iP1,
                                const  T&  iP2 )
    {
        return  iP0 * 0.25f +
                iP1 * 0.5f  +
                iP2 * 0.25f;
    }

    static
    float
    QuadraticBezierSampledLengthApproximation( const  FVector2D&  iP0,
                                               const  FVector2D&  iP1,
                                               const  FVector2D&  iP2,
                                               int iSteps = 100 )
    {
        FVector2D last = iP0;
        float step = 1.f / iSteps;
        float dist = 0.f;
        for( int i = 1; i <= iSteps; ++i )
        {
            float t = i * step;
            FVector2D next =  QuadraticBezierPointAtParameter( iP0, iP1, iP2, t );
            dist += FVector2D::Distance( last, next );
            last = next;
        }
        return  dist;
    }

    static
    float
    QuadraticBezierRecursiveAdaptativeSampledLengthApproximation( const  FVector2D&  iP0,
                                                                  const  FVector2D&  iP1,
                                                                  const  FVector2D&  iP2,
                                                                  int iAdaptativeLength = 3 )
    {
        return  _QuadraticBezierRecursiveAdaptativeSampledLengthApproximation_Imp( iP0, iP1, iP2, iAdaptativeLength );
    }

    static
    float
    _QuadraticBezierRecursiveAdaptativeSampledLengthApproximation_Imp( const  FVector2D&  iP0,
                                                                       const  FVector2D&  iP1,
                                                                       const  FVector2D&  iP2,
                                                                       int iAdaptativeLength = 3 )
    {
        FVector2D mid = QuadraticBezierMiddlePoint( iP0, iP1, iP2 );
        float lengthSegmentA = FVector2D::Distance( iP0, mid );
        float lengthSegmentB = FVector2D::Distance( mid, iP2 );
        if( lengthSegmentA > float( iAdaptativeLength ) )
        {
            FVector2D A = iP0;
            FVector2D B = iP1;
            FVector2D C = iP2;
            QuadraticBezierSplitMiddle( &A, &B, &C );
            lengthSegmentA = _QuadraticBezierRecursiveAdaptativeSampledLengthApproximation_Imp( A, B, C, iAdaptativeLength );
        }

        if( lengthSegmentB > float( iAdaptativeLength ) )
        {
            FVector2D D = iP0;
            FVector2D E = iP1;
            FVector2D F = iP2;
            QuadraticBezierInverseSplitMiddle( &D, &E, &F );
            lengthSegmentB = _QuadraticBezierRecursiveAdaptativeSampledLengthApproximation_Imp( D, E, F, iAdaptativeLength );
        }

        return  lengthSegmentA + lengthSegmentB;
    }

    static
    float
    QuadraticBezierLengthAtParameter( const  FVector2D&  iP0,
                                      const  FVector2D&  iP1,
                                      const  FVector2D&  iP2,
                                      float  t )
    {
        FVector2D A = iP0;
        FVector2D B = iP1;
        FVector2D C = iP2;
        QuadraticBezierSplitAtParameter( &A, &B, &C, t );
        return  QuadraticBezierRecursiveAdaptativeSampledLengthApproximation( A, B, C );
    }

    static
    float
    QuadraticBezierLinearParameterAtParameter( const  FVector2D&  iP0,
                                               const  FVector2D&  iP1,
                                               const  FVector2D&  iP2,
                                               float  t )
    {
        if( t == 1.f )  return  1.f;
        if( t == 0.5f ) return  0.5f;
        if( t == 0.f )  return  0.f;

        float length = QuadraticBezierRecursiveAdaptativeSampledLengthApproximation( iP0, iP1, iP2 );
        float lengthParam = QuadraticBezierLengthAtParameter( iP0, iP1, iP2, t );
        return  length / lengthParam;
    }

    static
    float
    QuadraticBezierGenerateLinearLUT( TArray< FOdysseyBezierLutElement >* ioArray,
                                      const  FVector2D&  iP0,
                                      const  FVector2D&  iP1,
                                      const  FVector2D&  iP2,
                                      int iAdaptativeLength = 3 )
    {
        ioArray->Empty();
        ioArray->Add( { iP0, 0.f } );
        float length = QuadraticBezierGenerateLinearLUT_Imp( ioArray, iP0, iP1, iP2, iAdaptativeLength );
        ioArray->Add( { iP2, length } );
        return  length;
    }


    static
    float
    QuadraticBezierGenerateLinearLUT_Imp( TArray< FOdysseyBezierLutElement >* ioArray,
                                          const  FVector2D&  iP0,
                                          const  FVector2D&  iP1,
                                          const  FVector2D&  iP2,
                                          int iAdaptativeLength = 3,
                                          float iOffset = 0.f )
    {
        FVector2D mid = QuadraticBezierMiddlePoint( iP0, iP1, iP2 );
        float lengthSegmentA = FVector2D::Distance( iP0, mid );
        float lengthSegmentB = FVector2D::Distance( mid, iP2 );

        if( lengthSegmentA > float( iAdaptativeLength ) )
        {
            FVector2D  A = iP0;
            FVector2D  B = iP1;
            FVector2D  C = iP2;
            QuadraticBezierSplitMiddle( &A, &B, &C );
            lengthSegmentA = QuadraticBezierGenerateLinearLUT_Imp( ioArray, A, B, C, iAdaptativeLength, iOffset );
        }

        ioArray->Add( { mid, ( lengthSegmentA + iOffset ) } );

        if( lengthSegmentB > float( iAdaptativeLength ) )
        {
            FVector2D  D = iP0;
            FVector2D  E = iP1;
            FVector2D  F = iP2;
            QuadraticBezierInverseSplitMiddle( &D, &E, &F );
            lengthSegmentB = QuadraticBezierGenerateLinearLUT_Imp( ioArray, D, E, F, iAdaptativeLength, iOffset + lengthSegmentA );
        }

        return  ( lengthSegmentA + lengthSegmentB );
    }
};

