// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

struct FOdysseyMathUtils
{
    struct FOdysseyBezierLutElement
    {
        FVector2D point;
        float length;
    };

    template< class T >
    static T CubicBezierPointAtParameter( const T& iP0, const T& iP1, const T& iP2, const T& iP3, float t );

    template< class T >
    static T QuadraticBezierPointAtParameter( const T& iP0, const T& iP1, const T& iP2, float t );

    static void QuadraticBezierSplitAtParameter( FVector2D* ioP0, FVector2D* ioP1, FVector2D* ioP2, float t );
    static void QuadraticBezierInverseSplitAtParameter( FVector2D* ioP0, FVector2D* ioP1, FVector2D* ioP2, float t );
    static void QuadraticBezierSplitMiddle( FVector2D* ioP0, FVector2D* ioP1, FVector2D* ioP2 );
    static void QuadraticBezierInverseSplitMiddle( FVector2D* ioP0, FVector2D* ioP1, FVector2D* ioP2 );

    template< class T >
    static T QuadraticBezierMiddlePoint( const T& iP0, const T& iP1, const T& iP2 );

    static float QuadraticBezierSampledLengthApproximation( const FVector2D& iP0, const FVector2D& iP1, const FVector2D& iP2, int iSteps = 100 );
    static float QuadraticBezierRecursiveAdaptativeSampledLengthApproximation( const FVector2D& iP0, const FVector2D& iP1, const FVector2D& iP2, int iAdaptativeLength = 3 );
    static float _QuadraticBezierRecursiveAdaptativeSampledLengthApproximation_Imp( const FVector2D& iP0, const FVector2D& iP1, const FVector2D& iP2, int iAdaptativeLength = 3 );

    static float QuadraticBezierLengthAtParameter( const FVector2D& iP0, const FVector2D& iP1, const FVector2D& iP2, float t );
    static float QuadraticBezierLinearParameterAtParameter( const FVector2D& iP0, const FVector2D& iP1, const FVector2D& iP2, float t );

    static float QuadraticBezierGenerateLinearLUT( TArray< FOdysseyBezierLutElement >* ioArray, const FVector2D& iP0, const FVector2D& iP1, const FVector2D& iP2, int iAdaptativeLength = 3 );
    static float QuadraticBezierGenerateLinearLUT_Imp( TArray< FOdysseyBezierLutElement >* ioArray, const FVector2D& iP0, const FVector2D& iP1, const FVector2D& iP2, int iAdaptativeLength = 3, float iOffset = 0.f );
};

#include "OdysseyMathUtils.inl"
