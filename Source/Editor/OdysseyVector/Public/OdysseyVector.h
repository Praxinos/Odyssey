#pragma once

#include <ULIS>
#include <blend2d.h>

namespace FOdysseyVector
{
    void ODYSSEYVECTOR_API PrintMatrix( char* name, BLMatrix2D& matrix );
    void ODYSSEYVECTOR_API MatrixMultiply( BLMatrix2D& iA, BLMatrix2D& iB, BLMatrix2D& oOut );
    void ODYSSEYVECTOR_API ExtractTransformations( BLMatrix2D &iMatrix
                                                 , double* iTranslationX
                                                 , double* iTranslationY
                                                 , double* iRotation
                                                 , double* iScalingX
                                                 , double* iScalingY );

    bool ODYSSEYVECTOR_API IntersectSegment( const ::ULIS::FVec2D& line0p0
                                                   , const ::ULIS::FVec2D& line0p1
                                                   , const ::ULIS::FVec2D& line1p0
                                                   , const ::ULIS::FVec2D& line1p1
                                                   , double* line0t
                                                   , double* line1t );

    double ODYSSEYVECTOR_API Cross2D( const ::ULIS::FVec2D& iA, const ::ULIS::FVec2D &iB );

    double ODYSSEYVECTOR_API DistanceToSegment( const ::ULIS::FVec2D& iPt
                                              , const ::ULIS::FVec2D& iSegmentP0
                                              , const ::ULIS::FVec2D& iSegmentP1
                                              , double& oDistance );

    void ODYSSEYVECTOR_API BezierExtract( ::ULIS::FVec2D& iP0
                                        , ::ULIS::FVec2D& iP1
                                        , ::ULIS::FVec2D& iP2
                                        , ::ULIS::FVec2D& iP3
                                        , double fromT
                                        , double toT
                                        , ::ULIS::FVec2D& oP0
                                        , ::ULIS::FVec2D& oP1
                                        , ::ULIS::FVec2D& oP2
                                        , ::ULIS::FVec2D& oP3 );

    bool ODYSSEYVECTOR_API PickBezier( const ::ULIS::FVec2D iWorldBezier[4]
                                     , const ::ULIS::FRectD& iMaskRect
                                     , const uint8* iPixelData );

    double ODYSSEYVECTOR_API GetBezierApproximateLength( ::ULIS::FVec2D iBezier[4], uint32 iDivisions );

    bool ProjectPoint( const ::ULIS::FVec2D& iPt
                     , const ::ULIS::FVec2D& iSegmentP0
                     , const ::ULIS::FVec2D& iSegmentP1
                     ,       ::ULIS::FVec2D& oProjected );

    template< typename T >
    bool IntersectRegions( const ::ULIS::TRectangle<T>& iRegion0
                         , const ::ULIS::TRectangle<T>& iRegion1
                         ,       ::ULIS::TRectangle<T>* oRegionOut )
    {
        ::ULIS::TRectangle<T> resultRegion;

        T r0x1 = iRegion0.x;
        T r0x2 = iRegion0.x + iRegion0.w;
        T r0y1 = iRegion0.y;
        T r0y2 = iRegion0.y + iRegion0.h;
        T r1x1 = iRegion1.x;
        T r1x2 = iRegion1.x + iRegion1.w;
        T r1y1 = iRegion1.y;
        T r1y2 = iRegion1.y + iRegion1.h;
        T x1 = ::ULIS::FMath::Max( r0x1, r1x1 );
        T y1 = ::ULIS::FMath::Max( r0y1, r1y1 );
        T x2 = ::ULIS::FMath::Min( r0x2, r1x2 );
        T y2 = ::ULIS::FMath::Min( r0y2, r1y2 );

        // Note: the intersect operation from the operator overload & in class FRectI assumes x1 < x2, which is not guaranted.
        // this is why we need to check that first.
        resultRegion.x = x1;
        resultRegion.y = y1;
        resultRegion.w = ( x1 < x2 ) ? x2 - x1 : 0;
        resultRegion.h = ( y1 < y2 ) ? y2 - y1 : 0;

        if( oRegionOut )
        {
            *oRegionOut = resultRegion;
        }

        return resultRegion.Area() ? true : false;
    }

    //bool ODYSSEYVECTOR_API IntersectRegions( const ::ULIS::FRectI& iRegion0, const ::ULIS::FRectI& iRegion1, ::ULIS::FRectI &oRegionOut );
}

#include "OdysseyVectorPolygon.h"
#include "OdysseyVectorBrush.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorHandleSegment.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorLine.h"
#include "OdysseyVectorLink.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorRectangle.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCycle.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorPathTracer.h"
#include "OdysseyVectorEllipse.h"
#include "OdysseyVectorEngine.h"
