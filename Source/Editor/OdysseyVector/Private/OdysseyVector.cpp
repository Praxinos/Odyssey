#include "OdysseyVector.h"

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

double
FOdysseyVector::Cross2D( const ::ULIS::FVec2D& iA, const ::ULIS::FVec2D &iB )
{
    return ( iA.x * iB.y ) - ( iA.y * iB.x );
}

// https://stackoverflow.com/questions/35473936/find-whether-two-line-segments-intersect-or-not-in-c
bool
FOdysseyVector::IntersectSegment( ::ULIS::FVec2D& line0p0
                                , ::ULIS::FVec2D& line0p1
                                , ::ULIS::FVec2D& line1p0
                                , ::ULIS::FVec2D& line1p1
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

// https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
double
FOdysseyVector::DistanceToSegment( ::ULIS::FVec2D& iPt
                                 , ::ULIS::FVec2D& iSegmentP0
                                 , ::ULIS::FVec2D& iSegmentP1
                                 , double&         oDistance )
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

bool
FOdysseyVector::IntersectRegions( const ::ULIS::FRectI& iRegion0, const ::ULIS::FRectI& iRegion1, ::ULIS::FRectI &oRegionOut )
{
    ::ULIS::FRectI resultRegion;

    int r0x1 = iRegion0.x;
    int r0x2 = iRegion0.x + iRegion0.w;
    int r0y1 = iRegion0.y;
    int r0y2 = iRegion0.y + iRegion0.h;
    int r1x1 = iRegion1.x;
    int r1x2 = iRegion1.x + iRegion1.w;
    int r1y1 = iRegion1.y;
    int r1y2 = iRegion1.y + iRegion1.h;
    int x1 = ::ULIS::FMath::Max( r0x1, r1x1 );
    int y1 = ::ULIS::FMath::Max( r0y1, r1y1 );
    int x2 = ::ULIS::FMath::Min( r0x2, r1x2 );
    int y2 = ::ULIS::FMath::Min( r0y2, r1y2 );

    // Note: the intersect operation from the operator overload & in class FRectI assumes x1 < x2, which is not guaranted.
    // this is why we need to check that first.
    oRegionOut.x = x1;
    oRegionOut.y = y1;
    oRegionOut.w = ( x1 < x2 ) ? x2 - x1 : 0;
    oRegionOut.h = ( y1 < y2 ) ? y2 - y1 : 0;

    return oRegionOut.Area() ? true : false;
}
