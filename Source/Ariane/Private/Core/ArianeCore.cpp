// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeCore.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

// https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
double
FArianeCore::DistanceToSegment( const FVector2D& iPt
                              , const FVector2D& iSegmentP0
                              , const FVector2D& iSegmentP1
                              , double& oDistance )
{
    // Return minimum distance between line segment vw and point p
    FVector2D p0p1 = iSegmentP1 - iSegmentP0;
    double sqLength = p0p1.SquaredLength();  // i.e. |w-v|^2 -  avoid a sqrt

    // Consider the line extending the segment, parameterized as v + t (w - v).
    // We find projection of point p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    // We clamp t from [0,1] to handle points outside the segment vw.
    FVector2D p0pt = ( iPt - iSegmentP0 );
    double t = p0pt.Dot( p0p1 ) / sqLength;

    oDistance = DBL_MAX;

    if( ( t >= 0.0f ) && ( t <= 1.0f ) )
    {
        FVector2D projected = iSegmentP0 + ( t * p0p1 );  // Projection falls on the segment
        FVector2D projp = projected - iPt;

        oDistance = projp.Length();
    }

    return t;
}

double
FArianeCore::DistanceToSegmentConstrained( const FVector2D& iPt
                                         , const FVector2D& iSegmentP0
                                         , const FVector2D& iSegmentP1
                                         , double&         oDistance )
{
    double t = FArianeCore::DistanceToSegment( iPt, iSegmentP0, iSegmentP1, oDistance );

    if( t < 0.0f )
    {
        t = 0.0f;

        oDistance = ( iSegmentP0 - iPt ).Length();
    }


    if( t > 1.0f )
    {
        t = 1.0f;

        oDistance = ( iSegmentP1 - iPt ).Length();
    }

    return t;
}

float
FArianeCore::IntersectPlane ( const FPlane& iPlane
                            , const FVector& iOrigin
                            , const FVector& iDirection
                            , FVector& oOut )
{
    if( FVector::DotProduct(iDirection, iPlane) )
    {
        double T = FMath::RayPlaneIntersectionParam( iOrigin, iDirection, iPlane );

        oOut = iOrigin + ( iDirection * T );

        return T;
    }

    return -1.0f;

/* commented-out: Unreal uses Ax+By+Cz=W, hence W = -D. So we use unreal's plane API
    float vo = ( iPlane.X * iOrigin.X ) +
               ( iPlane.Y * iOrigin.Y ) +
               ( iPlane.Z * iOrigin.Z ) + iPlane.W,
          vd = ( iPlane.X * iDirection.X ) +
               ( iPlane.Y * iDirection.Y ) +
               ( iPlane.Z * iDirection.Z );
    float t;

    if ( vd == 0.0f ) return 0.0f;

    t = - ( vo / vd );

    if ( t > 0.0f )
    {
        oOut.X = iOrigin.X + ( iDirection.X * t );
        oOut.Y = iOrigin.Y + ( iDirection.Y * t );
        oOut.Z = iOrigin.Z + ( iDirection.Z * t );

        return t;
    }

    return 0.0f;
*/
}

// https://stackoverflow.com/questions/35473936/find-whether-two-line-segments-intersect-or-not-in-c
bool
FArianeCore::IntersectSegment( const FVector2D& Line0p0
                             , const FVector2D& Line0p1
                             , const FVector2D& Line1p0
                             , const FVector2D& Line1p1
                             , double* Line0t
                             , double* Line1t )
{
    FVector2D L0Vec   = { Line0p1.X - Line0p0.X, Line0p1.Y - Line0p0.Y };
    FVector2D L1Vec   = { Line1p0.X - Line1p1.X, Line1p0.Y - Line1p1.Y };
    FVector2D L0L1Vec = { Line1p0.X - Line0p0.X, Line1p0.Y - Line0p0.Y };

    double Det = L0Vec.X * L1Vec.Y - L0Vec.Y * L1Vec.X;

    if ( fabs(Det) == 0.0f ) return false;

    double R = ( L0L1Vec.X * L1Vec.Y   - L0L1Vec.Y * L1Vec.X   ) / Det;
    double S = (   L0Vec.X * L0L1Vec.Y -   L0Vec.Y * L0L1Vec.X ) / Det;

    if( Line0t ) *Line0t = R;
    if( Line1t ) *Line1t = S;

    return !(R < 0 || R > 1 || S < 0 || S > 1);
}
