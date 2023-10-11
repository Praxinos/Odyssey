#include "OdysseyVector.h"

// https://drafts.csswg.org/css-transforms/#decomposing-a-2d-matrix
void
FOdysseyVector::ExtractTransformations( BLMatrix2D &iMatrix
                                      , double* iTranslationX
                                      , double* iTranslationY
                                      , double* iRotation
                                      , double* iScalingX
                                      , double* iScalingY )
{
    double row0x = iMatrix.m00;
    double row0y = iMatrix.m01;
    double row1x = iMatrix.m10;
    double row1y = iMatrix.m11;
    double translation[2];
    double scale[2];

    translation[0] = iMatrix.m20;
    translation[1] = iMatrix.m21;

    scale[0] = sqrt( row0x * row0x + row0y * row0y );
    scale[1] = sqrt( row1x * row1x + row1y * row1y );

    // If determinant is negative, one axis was flipped.
    double determinant = row0x * row1y - row0y * row1x;

    if( determinant < 0 )
    {
        // Flip axis with minimum unit vector dot product.
        if ( row0x < row1y )
        {
            scale[0] = -scale[0];
        }
        else
        {
            scale[1] = -scale[1];
        }
    }

    // Renormalize matrix to remove scale.

    if( scale[0] )
    {
        row0x *= 1.0f / scale[0];
        row0y *= 1.0f / scale[0];
    }

    if( scale[1] )
    {
        row1x *= 1.0f / scale[1];
        row1y *= 1.0f / scale[1];
    }

    // Compute rotation and renormalize matrix.
    double angle = atan2( row0y, row0x );

    if( iTranslationX ) *iTranslationX = translation[0];
    if( iTranslationY ) *iTranslationY = translation[1];
    if( iRotation     ) *iRotation = angle;
    if( iScalingX     ) *iScalingX = scale[0];
    if( iScalingY     ) *iScalingY = scale[1];
}

/*
// https://stackoverflow.com/questions/45159314/decompose-2d-transformation-matrix
void
FOdysseyVector::ExtractTransformations( BLMatrix2D &iMatrix
                                      , double* iTranslationX
                                      , double* iTranslationY
                                      , double* iRotation
                                      , double* iScalingX
                                      , double* iScalingY )
{
    if( iTranslationX )
    {
        *iTranslationX = iMatrix.m20;
    }

    if( iTranslationY )
    {
        *iTranslationY = iMatrix.m21;
    }

    if( iRotation )
    {
        *iRotation = atan2( iMatrix.m01, iMatrix.m00 );
    }

    if( iScalingX )
    {
        *iScalingX = sqrt( ( iMatrix.m00 * iMatrix.m00 ) + ( iMatrix.m01 * iMatrix.m01 ) );
    }

    if( iScalingY )
    {
        *iScalingY = sqrt( ( iMatrix.m10 * iMatrix.m10 ) + ( iMatrix.m11 * iMatrix.m11 ) );
    }
}
*/

inline double
FOdysseyVector::Cross2D( const ::ULIS::FVec2D& iA, const ::ULIS::FVec2D &iB )
{
    return ( iA.x * iB.y ) - ( iA.y * iB.x );
}

// https://stackoverflow.com/questions/35473936/find-whether-two-line-segments-intersect-or-not-in-c
inline bool
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

// convenience function that does not alter arguments
void
FOdysseyVector::MatrixMultiply( BLMatrix2D& iA, BLMatrix2D& iB, BLMatrix2D& oOut )
{
    oOut = iA;

    oOut.transform( iB );
}

void
FOdysseyVector::PrintMatrix( char* name, BLMatrix2D& matrix )
{
    UE_LOG(LogTemp,Warning,TEXT("%s -- M00:%f M01:%f // M10:%f  M11:%f // M20:%f M21:%f"), name, matrix.m00, matrix.m01, matrix.m10, matrix.m11, matrix.m20, matrix.m21 );
}

void
FOdysseyVector::BezierExtract( ::ULIS::FVec2D& iP0
                             , ::ULIS::FVec2D& iP1
                             , ::ULIS::FVec2D& iP2
                             , ::ULIS::FVec2D& iP3
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

    toT = ( toT - fromT ) / ( 1.0f - fromT ); // adjust t

    ::ULIS::CubicBezierSplitAtParameter<::ULIS::FVec2D>( &oP0, &oP1, &oP2, &oP3, toT );
}

// De Casteljau algorithm. Not the fastest but for such task we didn't need speed.
// Very accurate though.
bool
FOdysseyVector::PickBezier( const ::ULIS::FVec2D iWorldBezier[4]
                          , const ::ULIS::FRectD& iMaskRect
                          , uint8* iPixelData )
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
FOdysseyVector::GetBezierApproximateLength( ::ULIS::FVec2D iBezier[4], uint32 iDivisions )
{
    ::ULIS::FVec2D p0 = iBezier[0];
    double step = 1.0f / iDivisions;
    double length = 0.0f;
    double t0 = 0.0f;

    for( uint32 i = 0; i < iDivisions; i++ )
    {
        double t1 = t0 + step;
        ::ULIS::FVec2D p1 = ::ULIS::CubicBezierTangentAtParameter<::ULIS::FVec2D>( iBezier[0]
                                                                                 , iBezier[1]
                                                                                 , iBezier[2]
                                                                                 , iBezier[3]
                                                                                 , t1 );
        length += ::ULIS::FVec2D( p1 - p0 ).Distance();

        t0 = t1;
        p0 = p1;
    }

    return length;
}
