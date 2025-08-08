// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVector.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

/* works better than the other methods I could find */
// https://stackoverflow.com/questions/4361242/extract-rotation-scale-values-from-2d-transformation-matrix
// https://frederic-wang.fr/decomposition-of-2d-transform-matrices.html
// https://stackoverflow.com/questions/12469770/get-skew-or-rotation-value-from-affine-transformation-matrix
// inspired by a method described by Frederic Wang.
void
FOdysseyVector::ExtractTransformations( const BLMatrix2D &iMatrix
                                      , double* oTranslationX
                                      , double* oTranslationY
                                      , double* oRotation
                                      , double* oScalingX
                                      , double* oScalingY
                                      , double* oSkewX
                                      , double* oSkewY
                                      , bool iDegree )
{
    double a = iMatrix.m00;
    double b = iMatrix.m01;
    double c = iMatrix.m10;
    double d = iMatrix.m11;
    double e = iMatrix.m20;
    double f = iMatrix.m21;
    double delta = a * d - b * c;

    if( oTranslationX ) *oTranslationX = iMatrix.m20;
    if( oTranslationY ) *oTranslationY = iMatrix.m21;

    // Apply the QR-like decomposition.
    if ( ( a != 0 ) || ( b != 0 ) )
    {
        double r = sqrt( ( a * a ) + ( b * b ) );

        if( oRotation ) *oRotation = b > 0 ? acos( a / r ) : -acos( a / r );
        if( oScalingX ) *oScalingX = r;
        if( oScalingY ) *oScalingY = delta / r;
        if( oSkewX ) *oSkewX = atan((a * c + b * d) / (r * r));
        if( oSkewY ) *oSkewY = 0;
    }
    else
    {
        if ( c != 0 || d != 0 )
        {
            double s = sqrt( c * c + d * d );

            if( oRotation ) *oRotation = M_PI / 2.0f - ( d > 0.0f ? acos( -c / s ) : -acos( c / s ) );
            if( oScalingX ) *oScalingX = delta / s;
            if( oScalingY ) *oScalingY = s;
            if( oSkewX ) *oSkewX = 0;
            if( oSkewY ) *oSkewY = atan((a * c + b * d) / (s * s));
        }
    }

    if( oRotation && iDegree )
    {
        *oRotation = *oRotation * 180.0f / M_PI;
    }
}

::ULIS::FRectD
FOdysseyVector::MapRect( const BLMatrix2D& iMatrix
                       , const ::ULIS::FRectD& iRect )
{
    BLPoint p0 = iMatrix.mapPoint( iRect.x          , iRect.y           );
    BLPoint p1 = iMatrix.mapPoint( iRect.x + iRect.w, iRect.y           );
    BLPoint p2 = iMatrix.mapPoint( iRect.x + iRect.w, iRect.y + iRect.h );
    BLPoint p3 = iMatrix.mapPoint( iRect.x          , iRect.y + iRect.h );
    ::ULIS::FRectD bbox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                    , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                    , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                    , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

    return bbox;
}

::ULIS::FVec2D
FOdysseyVector::MapPoint( const BLMatrix2D& iMatrix
                        , const ::ULIS::FVec2D& iPoint )
{
    BLPoint pt = iMatrix.mapPoint( iPoint.x, iPoint.y );

    return ::ULIS::FVec2D( pt.x, pt.y );
}

::ULIS::FVec2D
FOdysseyVector::MapVector( const BLMatrix2D& iMatrix
                         , const ::ULIS::FVec2D& iPoint )
{
    BLPoint pt = iMatrix.mapVector( iPoint.x, iPoint.y );

    return ::ULIS::FVec2D( pt.x, pt.y );
}

double
FOdysseyVector::Cross2D( const ::ULIS::FVec2D& iA, const ::ULIS::FVec2D &iB )
{
    return ( iA.x * iB.y ) - ( iA.y * iB.x );
}

// https://stackoverflow.com/questions/35473936/find-whether-two-line-segments-intersect-or-not-in-c
bool
FOdysseyVector::IntersectSegment( const ::ULIS::FVec2D& line0p0
                                , const ::ULIS::FVec2D& line0p1
                                , const ::ULIS::FVec2D& line1p0
                                , const ::ULIS::FVec2D& line1p1
                                , double* line0t
                                , double* line1t )
{
    ::ULIS::FVec2D L0Vec   = { line0p1.x - line0p0.x, line0p1.y - line0p0.y };
    ::ULIS::FVec2D L1Vec   = { line1p0.x - line1p1.x, line1p0.y - line1p1.y }; // reverted order is normal
    ::ULIS::FVec2D L0L1Vec = { line1p0.x - line0p0.x, line1p0.y - line0p0.y };

    double det = L0Vec.x * L1Vec.y - L0Vec.y * L1Vec.x;

    if ( fabs(det) == 0.0f ) return false;

    double r = ( L0L1Vec.x * L1Vec.y   - L0L1Vec.y * L1Vec.x   ) / det;
    double s = (   L0Vec.x * L0L1Vec.y -   L0Vec.y * L0L1Vec.x ) / det;

    if( line0t ) *line0t = r;
    if( line1t ) *line1t = s;

    return !(r < 0 || r > 1 || s < 0 || s > 1);
}

double
FOdysseyVector::CubicBezierHitTest( const ::ULIS::FVec2D& iPt
                                  , const ::ULIS::FVec2D& iBezier0
                                  , const ::ULIS::FVec2D& iBezier1
                                  , const ::ULIS::FVec2D& iBezier2
                                  , const ::ULIS::FVec2D& iBezier3
                                  , uint32 iDivisions
                                  , double iMinDistance )
{
    double t = 0.0f;
    double step = 1.0f / ( iDivisions + 1 );
    double minDistance = iMinDistance;
    double absoluteT = -1.0f;

    for( uint32 i = 0; i <= iDivisions; i++ )
    {
        double fragmentT0 = t;
        double fragmentT1 = t + step;;
        ::ULIS::FVec2D fragmentP0 = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( iBezier0
                                                                                       , iBezier1
                                                                                       , iBezier2
                                                                                       , iBezier3
                                                                                       , fragmentT0 );
        ::ULIS::FVec2D fragmentP1 = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( iBezier0
                                                                                       , iBezier1
                                                                                       , iBezier2
                                                                                       , iBezier3
                                                                                       , fragmentT1 );
        double distance;
        double relativeT = FOdysseyVector::DistanceToSegmentConstrained( iPt, fragmentP0, fragmentP1, distance );

        if( distance < minDistance )
        {
            minDistance = distance;

            absoluteT = fragmentT0 + ( ( fragmentT1 - fragmentT0 ) * relativeT );
        }

        t = fragmentT1;
    }

    return absoluteT;
}

double
FOdysseyVector::QuadraticBezierHitTest( const ::ULIS::FVec2D& iPt
                                      , const ::ULIS::FVec2D& iBezier0
                                      , const ::ULIS::FVec2D& iBezier1
                                      , const ::ULIS::FVec2D& iBezier2
                                      , uint32 iDivisions
                                      , double iMinDistance )
{
    double t = 0.0f;
    double step = 1.0f / ( iDivisions + 1 );
    double minDistance = iMinDistance;
    double absoluteT = -1.0f;

    for( uint32 i = 0; i <= iDivisions; i++ )
    {
        double fragmentT0 = t;
        double fragmentT1 = t + step;;
        ::ULIS::FVec2D fragmentP0 = ::ULIS::QuadraticBezierPointAtParameter<::ULIS::FVec2D>( iBezier0
                                                                                          , iBezier1
                                                                                          , iBezier2
                                                                                          , fragmentT0 );
        ::ULIS::FVec2D fragmentP1 = ::ULIS::QuadraticBezierPointAtParameter<::ULIS::FVec2D>( iBezier0
                                                                                          , iBezier1
                                                                                          , iBezier2
                                                                                          , fragmentT1 );
        double distance;
        double relativeT = FOdysseyVector::DistanceToSegmentConstrained( iPt, fragmentP0, fragmentP1, distance );

        if( distance < minDistance )
        {
            minDistance = distance;

            absoluteT = fragmentT0 + ( ( fragmentT1 - fragmentT0 ) * relativeT );
        }

        t = fragmentT1;
    }

    return absoluteT;
}

//static
bool
FOdysseyVector::ProjectPoint( const ::ULIS::FVec2D& iPt
                            , const ::ULIS::FVec2D& iSegmentP0
                            , const ::ULIS::FVec2D& iSegmentP1
                            ,       ::ULIS::FVec2D& oProjected )
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

    if( ( t >= 0.0f ) && ( t <= 1.0f ) )
    {
        oProjected = iSegmentP0 + ( t * p0p1 );  // Projection falls on the segment

        return true;
    }

    return false;
}

// https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
double
FOdysseyVector::DistanceToSegment( const ::ULIS::FVec2D& iPt
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
FOdysseyVector::DistanceToSegmentConstrained( const ::ULIS::FVec2D& iPt
                                            , const ::ULIS::FVec2D& iSegmentP0
                                            , const ::ULIS::FVec2D& iSegmentP1
                                            , double&         oDistance )
{
    double t = FOdysseyVector::DistanceToSegment( iPt, iSegmentP0, iSegmentP1, oDistance );

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

// convenience function that does not alter arguments
void
FOdysseyVector::MatrixMultiply( const BLMatrix2D& iA, const BLMatrix2D& iB, BLMatrix2D& oOut )
{
    oOut = iA;

    oOut.transform( iB );
}

void
FOdysseyVector::PrintMatrix( char* name, BLMatrix2D& matrix )
{
    UE_LOG(LogTemp,Warning,TEXT("%hs -- M00:%f M01:%f // M10:%f  M11:%f // M20:%f M21:%f"), name, matrix.m00, matrix.m01, matrix.m10, matrix.m11, matrix.m20, matrix.m21 );
}

void
FOdysseyVector::BezierExtract( const ::ULIS::FVec2D& iP0
                             , const ::ULIS::FVec2D& iP1
                             , const ::ULIS::FVec2D& iP2
                             , const ::ULIS::FVec2D& iP3
                             , double fromT
                             , double toT
                             , ::ULIS::FVec2D& oP0
                             , ::ULIS::FVec2D& oP1
                             , ::ULIS::FVec2D& oP2
                             , ::ULIS::FVec2D& oP3 )
{
    oP0 = iP0;
    oP1 = iP1;
    oP2 = iP2;
    oP3 = iP3;

    ::ULIS::CubicBezierInverseSplitAtParameter<::ULIS::FVec2D>( &oP0, &oP1, &oP2, &oP3, fromT );

    toT = ( fromT == 1.0f ) ? 1.0f : ( toT - fromT ) / ( 1.0f - fromT ); // adjust t

    ::ULIS::CubicBezierSplitAtParameter<::ULIS::FVec2D>( &oP0, &oP1, &oP2, &oP3, toT );
}

// De Casteljau algorithm. Not the fastest but for such task we didn't need speed.
// Very accurate though.
bool
FOdysseyVector::PickBezier( const ::ULIS::FVec2D iWorldBezier[4]
                          , const ::ULIS::FRectD& iMaskRect
                          , const uint8* iPixelData )
{
    double xmin = ::ULIS::FMath::Min4( iWorldBezier[0].x, iWorldBezier[1].x
                                     , iWorldBezier[2].x, iWorldBezier[3].x );
    double ymin = ::ULIS::FMath::Min4( iWorldBezier[0].y, iWorldBezier[1].y
                                     , iWorldBezier[2].y, iWorldBezier[3].y );
    double xmax = ::ULIS::FMath::Max4( iWorldBezier[0].x, iWorldBezier[1].x
                                     , iWorldBezier[2].x, iWorldBezier[3].x );
    double ymax = ::ULIS::FMath::Max4( iWorldBezier[0].y, iWorldBezier[1].y
                                     , iWorldBezier[2].y, iWorldBezier[3].y );

    ::ULIS::FRectD bezierRect = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
    ::ULIS::FRectD intersectRect = bezierRect & iMaskRect;
/*
    if( intersectRect.Area() )
    {
*/
        if( ( fabs( iWorldBezier[0].x - iWorldBezier[1].x ) < 1.0f )
         && ( fabs( iWorldBezier[0].x - iWorldBezier[2].x ) < 1.0f )
         && ( fabs( iWorldBezier[0].x - iWorldBezier[3].x ) < 1.0f )
         && ( fabs( iWorldBezier[0].y - iWorldBezier[1].y ) < 1.0f )
         && ( fabs( iWorldBezier[0].y - iWorldBezier[2].y ) < 1.0f )
         && ( fabs( iWorldBezier[0].y - iWorldBezier[3].y ) < 1.0f ) )
        {
            int32 x = (int)iWorldBezier[0].x;
            int32 y = (int)iWorldBezier[0].y;

            if( ( x >= 0 ) && ( x < (int)iMaskRect.w ) && ( y >= 0 ) && ( y < (int)iMaskRect.h ) )
            {
                uint32 offset = ( y * iMaskRect.w ) + x;

                if ( iPixelData[offset] != 0 )
                {
                    return true;
                }
            }
        }
        else // refine
        {
            ::ULIS::FVec2D childBezier[2][4];

            memcpy( childBezier[0], iWorldBezier, sizeof( childBezier[0] ) );
            memcpy( childBezier[1], iWorldBezier, sizeof( childBezier[1] ) );

            ::ULIS::CubicBezierSplitAtParameter       <::ULIS::FVec2D>( &childBezier[0][0]
                                                                      , &childBezier[0][1]
                                                                      , &childBezier[0][2]
                                                                      , &childBezier[0][3]
                                                                      , 0.5f );
            if( PickBezier( childBezier[0], iMaskRect, iPixelData ) == true )
            {
                return true;
            }

            ::ULIS::CubicBezierInverseSplitAtParameter<::ULIS::FVec2D>( &childBezier[1][0]
                                                                      , &childBezier[1][1]
                                                                      , &childBezier[1][2]
                                                                      , &childBezier[1][3]
                                                                      , 0.5f );
            if( PickBezier( childBezier[1], iMaskRect, iPixelData ) == true )
            {
                return true;
            }
        }
/*
    }
*/
    return false;
}

double
FOdysseyVector::GetCubicBezierApproximateLength( const ::ULIS::FVec2D iBezier[4]
                                               , uint32 iDivisions )
{
    return GetCubicBezierApproximateLength( iBezier, iDivisions, nullptr );
}

double
FOdysseyVector::GetCubicBezierApproximateLength( const ::ULIS::FVec2D iBezier[4]
                                               , uint32 iDivisions
                                               , std::vector<double>* oDivisionLengthBuffer )
{
    ::ULIS::FVec2D p0 = iBezier[0];
    double step = 1.0f / iDivisions;
    double length = 0.0f;
    double t0 = 0.0f;

    if( oDivisionLengthBuffer )
        oDivisionLengthBuffer->resize( iDivisions );

    for( uint32 i = 0; i < iDivisions; i++ )
    {
        double t1 = t0 + step;
        ::ULIS::FVec2D p1 = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( iBezier[0]
                                                                               , iBezier[1]
                                                                               , iBezier[2]
                                                                               , iBezier[3]
                                                                               , t1 );
        double fractionLength = ::ULIS::FVec2D( p1 - p0 ).Distance();

        if( oDivisionLengthBuffer )
        {
            (*oDivisionLengthBuffer)[i] = fractionLength;
        }

        length += fractionLength;

        t0 = t1;
        p0 = p1;
    }

    return length;
}

double
FOdysseyVector::GetQuadraticBezierApproximateLength( const ::ULIS::FVec2D iBezier[4]
                                                   , uint32 iDivisions )
{
    return GetQuadraticBezierApproximateLength( iBezier, iDivisions, nullptr, nullptr );
}

double
FOdysseyVector::GetQuadraticBezierApproximateLength( const ::ULIS::FVec2D iBezier[4]
                                                   , uint32 iDivisions
                                                   , std::vector<double>* oDivisionLengthBuffer
                                                   , std::vector<::ULIS::FVec2D>* oDivisionPointBuffer )
{
    ::ULIS::FVec2D p0 = iBezier[0];
    double step = 1.0f / iDivisions;
    double length = 0.0f;
    double t0 = 0.0f;

    if( oDivisionLengthBuffer )
    {
        oDivisionLengthBuffer->resize( iDivisions );
    }

    if( oDivisionPointBuffer )
    {
        oDivisionPointBuffer->resize( iDivisions + 1 );

        (*oDivisionPointBuffer)[0] = p0;
    }

    for( uint32 i = 0; i < iDivisions; i++ )
    {
        uint32 n = i + 1;
        double t1 = t0 + step;
        ::ULIS::FVec2D p1 = ::ULIS::QuadraticBezierPointAtParameter<::ULIS::FVec2D>( iBezier[0]
                                                                                   , iBezier[1]
                                                                                   , iBezier[2]
                                                                                   , t1 );
        double fractionLength = ::ULIS::FVec2D( p1 - p0 ).Distance();

        if( oDivisionLengthBuffer )
        {
            (*oDivisionLengthBuffer)[i] = fractionLength;
        }

        if( oDivisionPointBuffer )
        {
            (*oDivisionPointBuffer)[n] = p1;
        }

        length += fractionLength;

        t0 = t1;
        p0 = p1;
    }

    return length;
}

//////////////////////// Curve fitting //////////////////////////////////////
// C++ Adaptation of the algorithm for Automatically Fitting Digitized Curves
// by Philip J. Schneider
// "Graphics Gems", Academic Press, 1990
// Adaptation by Gary GABRIEL @ Praxinos. 2024
// https://github.com/erich666/GraphicsGems/blob/master/gems/FitCurves.c

#define MAXPOINTS 256

/*
 *  B0, B1, B2, B3 :
 *    Bezier multipliers
 */
static double B0( double u )
{
    double tmp = 1.0f - u;

    return ( tmp * tmp * tmp );
}

static double B1( double u )
{
    double tmp = 1.0f - u;

    return ( 3.0f * u * ( tmp * tmp ) );
}

static double B2( double u )
{
    double tmp = 1.0f - u;

    return ( 3.0f * u * u * tmp );
}

static double B3( double u )
{
    return ( u * u * u );
}

/*
 *  Bezier :
 *      Evaluate a Bezier curve at a particular parameter value
*     int        degree; // The degree of the bezier curve
    Point2     *V; // Array of control points
    double     t; // Parametric value to find point for
 *
 */
static ::ULIS::FVec2D BezierII( uint32 degree
                              , const std::vector<::ULIS::FVec2D>& V
                              , double t )
{
    //::ULIS::FVec2D     Q;            /* Point on curve at parameter t    */
    //std::vector<::ULIS::FVec2D> Vtemp;/* Local copy of control points        */

    return ::ULIS::CubicBezierPointAtParameter( V[0], V[1], V[2], V[3], t );
/*
    Vtemp.resize( degree + 1 );

    for ( uint32 i = 0; i <= degree; i++ )
    {
        Vtemp[i] = V[i];
    }

    // Triangle computation
    for ( uint32 i = 1; i <= degree; i++ )
    {
        for ( uint32 j = 0; j <= ( degree - i ); j++ )
        {
            Vtemp[j].x = ( 1.0f - t ) * Vtemp[j].x + t * Vtemp[j+1].x;
            Vtemp[j].y = ( 1.0f - t ) * Vtemp[j].y + t * Vtemp[j+1].y;
        }
    }

    Q = Vtemp[0];

    return Q;
*/
}

/*
 *  ComputeMaxError :
 *    Find the maximum squared distance of digitized points
 *    to fitted curve.
*     Point2    *d;       // Array of digitized points
    int        first, last;  // Indices defining region
    BezierCurve    bezCurve; // Fitted Bezier curve
    double    *u;           // Parameterization of points
    int        *splitPoint;  // Point of maximum error
*/
static double
ComputeMaxError( const std::vector<::ULIS::FVec2D>& iPointBuffer
               , const ::ULIS::FVec2D* iFirstRecord
               , const ::ULIS::FVec2D* iLastRecord
               , const std::vector<::ULIS::FVec2D>& iBezierCurve
               , const std::vector<double>& u
               , ::ULIS::FVec2D** splitPoint )

{
    uint32 firstRecordIndex = iFirstRecord - &iPointBuffer[0];
    uint32 lastRecordIndex  = iLastRecord  - &iPointBuffer[0];
    double    maxDist; // Maximum error
    double    dist; // Current error
    ::ULIS::FVec2D    P; // Point on curve
    ::ULIS::FVec2D    v; // Vector from point to curve

    *splitPoint = (::ULIS::FVec2D*) &iPointBuffer[ ( iLastRecord - iFirstRecord + 1 ) / 2];

    maxDist = 0.0;

    for ( uint32 i = firstRecordIndex + 1, j = 1; i < lastRecordIndex; i++, j++ )
    {
        P = BezierII( 3, iBezierCurve, u[j] );
        v = P - iPointBuffer[i];

        dist = v.DistanceSquared();

        if ( dist >= maxDist )
        {
            maxDist = dist;

            *splitPoint = (::ULIS::FVec2D*) &iPointBuffer[i];
        }
    }
    return (maxDist);
}

static ::ULIS::FVec2D
ComputeCenterTangent( const std::vector<::ULIS::FVec2D>& iPointBuffer
                    , const ::ULIS::FVec2D* iCenter )
{
    ::ULIS::FVec2D    V1, V2, tHatCenter;

    V1 = (*(iCenter - 1)) - (*iCenter);
    V2 = (*iCenter)       - (*(iCenter + 1));

    tHatCenter.x = ( V1.x + V2.x ) / 2.0f;
    tHatCenter.y = ( V1.y + V2.y ) / 2.0f;

    tHatCenter.Normalize();

    return tHatCenter;
}

/*
 *  ChordLengthParameterize :
 *    Assign parameter values to digitized points
 *    using relative distances between points.
 *     Point2    *d; // Array of digitized points
    int        first, last; // Indices defining region
 */
void
FOdysseyVector::ChordLengthParameterize( const std::vector<::ULIS::FVec2D>& iPointBuffer
                                       , const ::ULIS::FVec2D* iFirstRecord
                                       , const ::ULIS::FVec2D* iLastRecord
                                       , std::vector<double>& oUBuffer )

{
    uint32 firstRecordIndex = iFirstRecord - &iPointBuffer[0];
    uint32 lastRecordIndex  = iLastRecord  - &iPointBuffer[0];
    uint32 nPts = ( iLastRecord - iFirstRecord ) + 1;

    oUBuffer.clear();
    oUBuffer.resize( nPts );

    oUBuffer[0] = 0.0f;

    for ( uint32 i = firstRecordIndex + 1, j = 1; i <= lastRecordIndex; i++, j++ )
    {
        oUBuffer[j] = oUBuffer[j-1] + ( iPointBuffer[i] - iPointBuffer[i-1] ).Distance();
    }

    for ( uint32 i = firstRecordIndex + 1, j = 1; i <= lastRecordIndex; i++, j++ )
    {
        oUBuffer[j] = oUBuffer[j] / oUBuffer[nPts-1];
    }
}

/*
 *  NewtonRaphsonRootFind :
 *    Use Newton-Raphson iteration to find better root.
*   BezierCurve Q; // Current fitted curve
    Point2 P; //  Digitized point
    double u; //  Parameter value for "P"
 */
static double NewtonRaphsonRootFind( const std::vector<::ULIS::FVec2D>& Q
                                   , const ::ULIS::FVec2D& P
                                   , double u )
{
    double numerator, denominator;
    std::vector<::ULIS::FVec2D> Q1, Q2;    // Q' and Q''
    ::ULIS::FVec2D Q_u, Q1_u, Q2_u; // u evaluated at Q, Q', & Q''
    double uPrime;    // Improved u

    Q1.resize( 3 );
    Q2.resize( 2 );

    /* Compute Q(u)    */
    Q_u = BezierII( 3, Q, u );

    /* Generate control vertices for Q'    */
    for ( uint32 i = 0; i <= 2; i++ )
    {
        Q1[i].x = ( Q[i+1].x - Q[i].x ) * 3.0f;
        Q1[i].y = ( Q[i+1].y - Q[i].y ) * 3.0f;
    }

    /* Generate control vertices for Q'' */
    for ( uint32 i = 0; i <= 1; i++)
    {
        Q2[i].x = ( Q1[i+1].x - Q1[i].x ) * 2.0;
        Q2[i].y = ( Q1[i+1].y - Q1[i].y ) * 2.0;
    }

    /* Compute Q'(u) and Q''(u)    */
    Q1_u = BezierII( 2, Q1, u );
    Q2_u = BezierII( 1, Q2, u );

    /* Compute f(u)/f'(u) */
    numerator = ( Q_u.x - P.x ) * ( Q1_u.x ) + ( Q_u.y - P.y ) * ( Q1_u.y );
    denominator = ( Q1_u.x ) * ( Q1_u.x ) + ( Q1_u.y ) * ( Q1_u.y ) +
                    ( Q_u.x - P.x ) * ( Q2_u.x ) + ( Q_u.y - P.y ) * ( Q2_u.y );

    if ( denominator == 0.0f ) return u;

    /* u = u - f(u)/f'(u) */
    uPrime = u - ( numerator / denominator );

    return (uPrime);
}

/*
 *  Reparameterize:
 *    Given set of points and their parameterization, try to find
 *   a better parameterization.
 *  Point2    *d;       // Array of digitized points
    int        first, last;  //  Indices defining region
    double    *u;           //  Current parameter values
    BezierCurve    bezCurve; //  Current fitted curve
 */
static void
Reparameterize( const std::vector<::ULIS::FVec2D>& iPointBuffer
              , const ::ULIS::FVec2D* iFirstRecord
              , const ::ULIS::FVec2D* iLastRecord
              , const std::vector<::ULIS::FVec2D>& iBezier
              , const std::vector<double>& iUBuffer
              , std::vector<double>& oUPrimeBuffer )
{
    uint32 firstRecordIndex = iFirstRecord - &iPointBuffer[0];
    uint32 lastRecordIndex  = iLastRecord  - &iPointBuffer[0];
    int nPts = ( lastRecordIndex - firstRecordIndex ) + 1;

    oUPrimeBuffer.resize( nPts );

    for ( uint32 i = firstRecordIndex, j = 0; i <= lastRecordIndex; i++, j++ )
    {
        oUPrimeBuffer[j] = NewtonRaphsonRootFind ( iBezier
                                                 , iPointBuffer[i]
                                                 , iUBuffer[j] );
    }
}

/*
 *  GenerateBezier :
 *  Use least-squares method to find Bezier control points for region.
*     Point2    *d;            //  Array of digitized points
    int        first, last;        //  Indices defining region
    double    *uPrime;        //  Parameter values for region
    Vector2    tHat1, tHat2;    //  Unit tangents at endpoints
 *
 */
void
FOdysseyVector::GenerateBezier( const std::vector<::ULIS::FVec2D>& iPointBuffer
                              , const ::ULIS::FVec2D* iFirstRecord
                              , const ::ULIS::FVec2D* iLastRecord
                              , const std::vector<double>& uPrime
                              , const ::ULIS::FVec2D& iLeftTangent
                              , const ::ULIS::FVec2D& iRightTangent
                              , ::ULIS::FVec2D oBezierOut[4] )
{
    ::ULIS::FVec2D     A[MAXPOINTS][2]; // Precomputed rhs for eqn
    uint32  nPts; // Number of pts in sub-curve
    double  C[2][2]; // Matrix C
    double  X[2]; //Matrix X
    double  det_C0_C1, // Determinants of matrices
            det_C0_X,
            det_X_C1;
    double  alpha_l, // Alpha values, left and right
            alpha_r;
    ::ULIS::FVec2D     tmp; // Utility variable
    double  segLength;
    double  epsilon;

    nPts = ( iLastRecord - iFirstRecord ) + 1;

    /* Compute the A's    */
    for ( uint32 i = 0; i < nPts; i++)
    {
        ::ULIS::FVec2D v1 = iLeftTangent
                     , v2 = iRightTangent;

        v1 *=  B1( uPrime[i] );
        v2 *=  B2( uPrime[i] );

        A[i][0] = v1;
        A[i][1] = v2;
    }

    /* Create the C and X matrices    */
    C[0][0] = 0.0;
    C[0][1] = 0.0;
    C[1][0] = 0.0;
    C[1][1] = 0.0;
    X[0]    = 0.0;
    X[1]    = 0.0;

    for ( uint32 i = 0; i < nPts; i++)
    {
        C[0][0] += A[i][0].DotProduct( A[i][0] );
        C[0][1] += A[i][0].DotProduct( A[i][1] );
        C[1][0] = C[0][1];
        C[1][1] += A[i][1].DotProduct( A[i][1] );

        tmp = ( (*(iFirstRecord+i))
               - ( ( (*iFirstRecord) * B0(uPrime[i]) )
                 + ( ( (*iFirstRecord) * B1(uPrime[i]) )
                   + ( ( (*iLastRecord) * B2(uPrime[i]) )
                     + ( (*iLastRecord) * B3(uPrime[i]) ) ) ) ) );


        X[0] += A[i][0].DotProduct( tmp );
        X[1] += A[i][1].DotProduct( tmp );
    }

    /* Compute the determinants of C and X    */
    det_C0_C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
    det_C0_X  = C[0][0] * X[1]    - C[1][0] * X[0];
    det_X_C1  = X[0]    * C[1][1] - X[1]    * C[0][1];

    /* Finally, derive alpha values    */
    alpha_l = (det_C0_C1 == 0) ? 0.0 : det_X_C1 / det_C0_C1;
    alpha_r = (det_C0_C1 == 0) ? 0.0 : det_C0_X / det_C0_C1;

    /* If alpha negative, use the Wu/Barsky heuristic (see text) */
    /* (if alpha is 0, you get coincident control points that lead to
     * divide by zero in any subsequent NewtonRaphsonRootFind() call. */
    segLength = ((*iLastRecord) - (*iFirstRecord)).Distance();
    epsilon = 1.0e-6 * segLength;

    if ( alpha_l < epsilon || alpha_r < epsilon )
    {
        /* fall back on standard (probably inaccurate) formula, and subdivide further if needed. */
        double dist = segLength / 3.0f;

        oBezierOut[0] = (*iFirstRecord);
        oBezierOut[3] = (*iLastRecord);

        oBezierOut[1] = oBezierOut[0] + ( iLeftTangent  * dist );
        oBezierOut[2] = oBezierOut[3] + ( iRightTangent * dist );

        return;
    }

    /*  First and last control points of the Bezier curve are */
    /*  positioned exactly at the first and last data points */
    /*  Control points 1 and 2 are positioned an alpha distance out */
    /*  on the tangent vectors, left and right, respectively */
    oBezierOut[0] = (*iFirstRecord);
    oBezierOut[3] = (*iLastRecord);

    oBezierOut[1] = oBezierOut[0] + ( iLeftTangent  * alpha_l );
    oBezierOut[2] = oBezierOut[3] + ( iRightTangent * alpha_r );
}

/*
 *  FitCubic :
 *      Fit a Bezier curve to a (sub)set of digitized points
*   Point2    *d;            //  Array of digitized points
    int        first, last;    // Indices of first and last pts in region
    Vector2    tHat1, tHat2;    // Unit tangent vectors at endpoints
    double    error;        //  User-defined error squared
 */
static void
FitCubic( const std::vector<::ULIS::FVec2D>& iPointBuffer
        , const ::ULIS::FVec2D* iFirstRecord
        , const ::ULIS::FVec2D* iLastRecord
        , double iFirstRecordT
        , double iLastRecordT
        , const ::ULIS::FVec2D& iLeftTangent
        , const ::ULIS::FVec2D& iRightTangent
        , double iError
        , std::function<void (const std::vector<::ULIS::FVec2D>&
                            , double
                            , double )> iFunction )

{
    std::vector<::ULIS::FVec2D> bezierCurve; // Control points of fitted Bezier curve
    std::vector<double> uBuffer; // Parameter values for point
    std::vector<double> uPrimeBuffer; // Improved parameter values
    double    maxError; // Maximum fitting error
    ::ULIS::FVec2D* splitPoint; // Point to split point set at
    double iterationError; // Error below which you try iterating
    uint32 maxIterations = 4; // Max times to try iterating
    ::ULIS::FVec2D tHatCenter; // Unit tangent vector at splitPoint
    int nPts; // Number of points in subset

    iterationError = iError * 4.0;    /* fixed issue 23 */
    nPts = ( iLastRecord - iFirstRecord ) + 1;

    bezierCurve.clear();
    bezierCurve.resize(4);

    //  Use heuristic if region only has two points in it
    if ( nPts == 2 )
    {
        double dist = ((*iLastRecord) - (*iFirstRecord)).Distance() / 3.0f;

        bezierCurve[0] = (*iFirstRecord);
        bezierCurve[3] = (*iLastRecord);

        bezierCurve[1] = bezierCurve[0] + ( iLeftTangent * dist );
        bezierCurve[2] = bezierCurve[3] + ( iRightTangent * dist );

        iFunction( bezierCurve, iFirstRecordT, iLastRecordT );

        return;
    }

    /*  Parameterize points, and attempt to fit curve */
    FOdysseyVector::ChordLengthParameterize( iPointBuffer
                                           , iFirstRecord
                                           , iLastRecord
                                           , uBuffer );

    FOdysseyVector::GenerateBezier( iPointBuffer
                                  , iFirstRecord
                                  , iLastRecord
                                  , uBuffer
                                  , iLeftTangent
                                  , iRightTangent
                                  , &bezierCurve[0] );

    /*  Find max deviation of points to fitted curve */
    maxError = ComputeMaxError( iPointBuffer
                              , iFirstRecord
                              , iLastRecord
                              , bezierCurve
                              , uBuffer
                              , &splitPoint );

    if ( maxError < iError )
    {
        iFunction( bezierCurve, iFirstRecordT, iLastRecordT );

        return;
    }

    /*  If error not too large, try some reparameterization  */
    /*  and iteration */
/* commented out for now: reparametrization gives strange values.
    if ( maxError < iterationError )
    {
        for ( uint32 i = 0; i < maxIterations; i++ )
        {
            Reparameterize( iPointBuffer
                          , iFirstRecord
                          , iLastRecord
                          , bezierCurve
                          , uBuffer
                          , uPrimeBuffer );

            GenerateBezier( iPointBuffer
                          , iFirstRecord
                          , iLastRecord
                          , uPrimeBuffer
                          , iLeftTangent
                          , iRightTangent
                          , bezierCurve );

            maxError = ComputeMaxError( iPointBuffer
                                      , iFirstRecord
                                      , iLastRecord
                                      , bezierCurve
                                      , uPrimeBuffer
                                      , &splitPoint );
            if ( maxError < iError )
            {
                iFunction( bezierCurve, iFirstRecordT, iLastRecordT );

                return;
            }

            uBuffer = uPrimeBuffer;
        }
    }
*/

    /* Fitting failed -- split at max error point and fit recursively */
    tHatCenter = ComputeCenterTangent( iPointBuffer, splitPoint );

    FitCubic( iPointBuffer
            , iFirstRecord
            , splitPoint
            , iFirstRecordT
            , (double) ( splitPoint - &iPointBuffer[0] ) / iPointBuffer.size()
            , iLeftTangent
            , tHatCenter
            , iError
            , iFunction );

    tHatCenter = - tHatCenter;

    FitCubic( iPointBuffer
            , splitPoint
            , iLastRecord
            , (double) ( splitPoint - &iPointBuffer[0] ) / iPointBuffer.size()
            , iLastRecordT
            , tHatCenter
            , iRightTangent
            , iError
            , iFunction );
}

/*
 *  FitCurve :
 *      Fit a Bezier curve to a set of digitized points
 *
 *   Point2    *d;            //  Array of digitized points
    int        nPts;        //  Number of digitized points
    double    error;        //  User-defined error squared
 */
void
FOdysseyVector::FitCurve( const std::vector<::ULIS::FVec2D>& iPointBuffer
                        , double iError
                        , std::function<void ( const std::vector<::ULIS::FVec2D>&
                                             , double
                                             , double )> iFunction )
{
    if( iPointBuffer.size() >= 2 )
    {
        const ::ULIS::FVec2D* firstRecord = &iPointBuffer.front();
        const ::ULIS::FVec2D* lastRecord  = &iPointBuffer.back();
        // Unit tangent vectors at endpoints
        ::ULIS::FVec2D leftTangent  = (*(firstRecord + 1 )) - (*firstRecord);
        ::ULIS::FVec2D rightTangent = (*(lastRecord  - 1 )) - (*lastRecord );

        if( leftTangent.DistanceSquared() && rightTangent.DistanceSquared() )
        {
            leftTangent.Normalize();
            rightTangent.Normalize();

            FitCubic( iPointBuffer
                    , firstRecord
                    , lastRecord
                    , 0.0f
                    , 1.0f
                    , leftTangent
                    , rightTangent
                    , iError
                    , iFunction );
        }
    }
}
