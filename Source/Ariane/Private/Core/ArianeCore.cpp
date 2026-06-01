// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeCore.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

// https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
double
FArianeCore::DistanceToSegment( const ::ULIS::FVec2D& iPt
                              , const ::ULIS::FVec2D& iSegmentP0
                              , const ::ULIS::FVec2D& iSegmentP1
                              , double& oDistance )
{
    // Return minimum distance between line segment vw and point p
    ::ULIS::FVec2D p0p1 = iSegmentP1 - iSegmentP0;
    double sqLength = p0p1.DistanceSquared();  // i.e. |w-v|^2 -  avoid a sqrt

    // Consider the line extending the segment, parameterized as v + t (w - v).
    // We find projection of point p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    // We clamp t from [0,1] to handle points outside the segment vw.
    ::ULIS::FVec2D p0pt = ( iPt - iSegmentP0 );
    double t = p0pt.DotProduct( p0p1 ) / sqLength;

    oDistance = DBL_MAX;

    if( ( t >= 0.0f ) && ( t <= 1.0f ) )
    {
        ::ULIS::FVec2D projected = iSegmentP0 + ( t * p0p1 );  // Projection falls on the segment
        ::ULIS::FVec2D projp = projected - iPt;

        oDistance = projp.Distance();
    }

    return t;
}

double
FArianeCore::DistanceToSegmentConstrained( const ::ULIS::FVec2D& iPt
                                         , const ::ULIS::FVec2D& iSegmentP0
                                         , const ::ULIS::FVec2D& iSegmentP1
                                         , double&         oDistance )
{
    double t = FArianeCore::DistanceToSegment( iPt, iSegmentP0, iSegmentP1, oDistance );

    if( t < 0.0f )
    {
        t = 0.0f;

        oDistance = ( iSegmentP0 - iPt ).Distance();
    }


    if( t > 1.0f )
    {
        t = 1.0f;

        oDistance = ( iSegmentP1 - iPt ).Distance();
    }

    return t;
}
