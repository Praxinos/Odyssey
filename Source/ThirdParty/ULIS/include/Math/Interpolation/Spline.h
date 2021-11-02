// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Spline.h
* @author       Clement Berthaud
* @brief        This file provides basic Spline tools.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/Rectangle.h"
#include "Math/Geometry/Vector.h"

ULIS_NAMESPACE_BEGIN
struct FSplineParametricSample {
    FVec2F point;
    float length;
    float param;
};

struct FSplineLinearSample {
    FVec2F point;
    float length;
    float param;
};
ULIS_NAMESPACE_END

