// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         CachedTransformInfo.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FCachedTransformInfo class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/Matrix.h"
#include "Math/Geometry/Vector.h"

ULIS_NAMESPACE_BEGIN

struct ULIS_API FTransformInfo
{
    FVec2F pivot;
    FVec2F translation;
    float rotation;
    FVec2F scale;
};

ULIS_NAMESPACE_END

