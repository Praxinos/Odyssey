#include "OdysseyVector.h"

double
FOdysseyVector::Cross2D( ::ULIS::FVec2D& iA, ::ULIS::FVec2D &iB )
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
