// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>

template<int size>
struct FOdysseyVectorPolygon {
    ::ULIS::FVec2D point[size];
    double U[size];  // TODO : convert to float to reduce memory footprint
    double V[size];  // TODO : convert to float to reduce memory footprint
    uint32 pointCount = size;  // TODO : remove reduce memory footprint
};

typedef FOdysseyVectorPolygon<3> FOdysseyVectorPolygon3; // total is 100 bytes
typedef FOdysseyVectorPolygon<4> FOdysseyVectorPolygon4; // total is 132 bytes
typedef FOdysseyVectorPolygon<5> FOdysseyVectorPolygon5; // total is 164 bytes
typedef FOdysseyVectorPolygon<6> FOdysseyVectorPolygon6; // total is 196 bytes
