// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <ULIS>

namespace FArianeCore
{
    double ARIANE_API DistanceToSegment( const FVector2D& iPt
                                       , const FVector2D& iSegmentP0
                                       , const FVector2D& iSegmentP1
                                       , double& oDistance );

    // This is a constrained version of a segment-to-point proximity test.
    // It means that if the projection of the point on the segment is beyond limits,
    // it will remain within limits (0.0f) or (1.0f). On the figure below, x would be at t=0.0
    //
    //                    ° (point)
    //     segment        |
    // 1______________0   x (proj. point will also be at t = 0.0,
    //                       even though it is outside the segment)
    //
    double ARIANE_API DistanceToSegmentConstrained( const FVector2D& iPt
                                                  , const FVector2D& iSegmentP0
                                                  , const FVector2D& iSegmentP1
                                                  , double&         oDistance );

    float ARIANE_API IntersectPlane ( const FPlane& iPlane
                                    , const FVector&  iOrigin
                                    , const FVector&  iDirection
                                    ,  FVector& oOut );

    bool ARIANE_API IntersectSegment( const FVector2D& Line0p0
                                    , const FVector2D& Line0p1
                                    , const FVector2D& Line1p0
                                    , const FVector2D& Line1p1
                                    , double* Line0t
                                    , double* Line1t );

    template< typename T >
    void BezierExtract( const T& InP0
                      , const T& InP1
                      , const T& InP2
                      , const T& InP3
                      , double FromT
                      , double ToT
                      , T& OutP0
                      , T& OutP1
                      , T& OutP2
                      , T& OutP3 )
    {
        OutP0 = InP0;
        OutP1 = InP1;
        OutP2 = InP2;
        OutP3 = InP3;

        ::ULIS::CubicBezierInverseSplitAtParameter<T>( &OutP0, &OutP1, &OutP2, &OutP3, FromT );

        ToT = ( FromT == 1.0f ) ? 1.0f : ( ToT - FromT ) / ( 1.0f - FromT ); // adjust T

        ::ULIS::CubicBezierSplitAtParameter<T>( &OutP0, &OutP1, &OutP2, &OutP3, ToT );
    }

    template< typename T >
    double GetCubicBezierApproximateLength( const T Bezier[4]
                                          , uint32 Divisions
                                          , TArray<double>* OutDivisionLengthBuffer )
    {
        T P0 = Bezier[0];
        double Step = 1.0f / Divisions;
        double Length = 0.0f;
        double T0 = 0.0f;

        if( OutDivisionLengthBuffer )
            OutDivisionLengthBuffer->SetNum( Divisions );

        for( uint32 i = 0; i < Divisions; i++ )
        {
            double T1 = T0 + Step;
            T P1 = ::ULIS::CubicBezierPointAtParameter<T>( Bezier[0]
                                                         , Bezier[1]
                                                         , Bezier[2]
                                                         , Bezier[3]
                                                         , T1 );

            double FractionLength = T( P1 - P0 ).Length();

            if( OutDivisionLengthBuffer )
            {
                (*OutDivisionLengthBuffer)[i] = FractionLength;
            }

            Length += FractionLength;

            T0 = T1;
            P0 = P1;
        }

        return Length;
    }

    template< typename T >
    double GetCubicBezierApproximateLength( const T Bezier[4], uint32 Divisions )
    {
        return GetCubicBezierApproximateLength<T>( Bezier, Divisions, nullptr );
    }
}
