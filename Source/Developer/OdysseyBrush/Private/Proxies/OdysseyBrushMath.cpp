// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Proxies/OdysseyBrushMath.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushMathLibrary"

//////////////////////////////////////////////////////////////////////////
// UOdysseyBrushMathLibrary
//--------------------------------------------------------------------------------------
//--------------------------------------------- Odyssey Brush Blueprint Callable Methods

//static
void
UOdysseyBrushMathLibrary::ConvertPolarToCartesianCoordinate( float AngleRad, float  Radius, float& DeltaX, float& DeltaY )
{
    float cosa = cos( AngleRad );
    float sina = sin( AngleRad );
    DeltaX = cosa * Radius;
    DeltaY = sina * Radius;
}

//static
void
UOdysseyBrushMathLibrary::ConvertCartesianToPolarCoordinate( float DeltaX, float DeltaY, float& AngleRad, float& Radius )
{
    AngleRad = atan2(DeltaY, DeltaX);
    Radius = sqrt(DeltaY * DeltaY + DeltaX * DeltaX);
}

#undef LOCTEXT_NAMESPACE

