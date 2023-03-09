#pragma once

#include <ULIS>

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

    double ODYSSEYVECTOR_API Cross2D( ::ULIS::FVec2D& iA, ::ULIS::FVec2D &iB );

    double ODYSSEYVECTOR_API DistanceToSegment( ::ULIS::FVec2D& iPt
                                              , ::ULIS::FVec2D& iSegmentP0
                                              , ::ULIS::FVec2D& iSegmentP1
                                              , double&         oDistance );
}

#include "OdysseyVectorObject.h"
#include "OdysseyVectorScene.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorHandle.h"
#include "OdysseyVectorHandlePoint.h"
#include "OdysseyVectorHandleSegment.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorVertexCubic.h"
#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorLink.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorRectangle.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorCycle.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorPathBuilder.h"
#include "OdysseyVectorPathCubic.h"
#include "OdysseyVectorEllipse.h"
#include "OdysseyVectorEngine.h"
#include "HUD/OdysseyVectorHUD.h"
#include "HUD/OdysseyVectorHUDLine.h"
