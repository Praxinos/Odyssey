#pragma once

#include <ULIS>

namespace FOdysseyVector
{
    bool IntersectSegment( ::ULIS::FVec2D& line0p0
                         , ::ULIS::FVec2D& line0p1
                         , ::ULIS::FVec2D& line1p0
                         , ::ULIS::FVec2D& line1p1
                         , double* line0t
                         , double* line1t );
}

#include "OdysseyVectorObject.h"
#include "OdysseyVectorRoot.h"
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
#include "OdysseyVectorLoop.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorPathBuilder.h"
#include "OdysseyVectorPathCubic.h"
#include "OdysseyVectorCircle.h"
#include "OdysseyVectorEngine.h"
#include "HUD/OdysseyVectorHUD.h"
#include "HUD/OdysseyVectorHUDLine.h"
