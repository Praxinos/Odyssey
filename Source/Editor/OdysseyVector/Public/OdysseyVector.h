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

    bool ODYSSEYVECTOR_API IntersectSegment( ::ULIS::FVec2D& line0p0
                                           , ::ULIS::FVec2D& line0p1
                                           , ::ULIS::FVec2D& line1p0
                                           , ::ULIS::FVec2D& line1p1
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

    bool ODYSSEYVECTOR_API IntersectRegions( const ::ULIS::FRectI& iRegion0, const ::ULIS::FRectI& iRegion1, ::ULIS::FRectI &oRegionOut );
}

#include "OdysseyVectorObject.h"
#include "OdysseyVectorScene.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorHandle.h"
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
#include "OdysseyVectorPathBuilder.h"
#include "OdysseyVectorPathCubic.h"
#include "OdysseyVectorEllipse.h"
#include "OdysseyVectorEngine.h"
#include "HUD/OdysseyVectorHUD.h"
#include "HUD/OdysseyVectorHUDPicking.h"
#include "HUD/OdysseyVectorHUDLine.h"
#include "HUD/OdysseyVectorHUDPathCubic.h"
#include "HUD/OdysseyVectorHUDEraser.h"
#include "HUD/OdysseyVectorHUDBucket.h"
#include "HUD/OdysseyVectorHUDPicking.h"
#include "HUD/OdysseyVectorHUDSelection.h"
#include "HUD/OdysseyVectorHUDRotate.h"
#include "HUD/OdysseyVectorHUDGrid.h"
