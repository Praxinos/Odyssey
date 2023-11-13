// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>

template<int size>
struct FOdysseyVectorPolygon {
    BLPoint point[size];
    double U[size];
    double V[size];
    uint32 pointCount = size;
};

typedef FOdysseyVectorPolygon<3> FOdysseyVectorPolygon3;
typedef FOdysseyVectorPolygon<4> FOdysseyVectorPolygon4;
typedef FOdysseyVectorPolygon<5> FOdysseyVectorPolygon5;
