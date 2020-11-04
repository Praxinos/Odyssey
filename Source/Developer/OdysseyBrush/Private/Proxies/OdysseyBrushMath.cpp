// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Proxies/OdysseyBrushMath.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushMathLibrary"

//////////////////////////////////////////////////////////////////////////
// UOdysseyBrushMathLibrary
//--------------------------------------------------------------------------------------
//--------------------------------------------- Odyssey Brush Blueprint Callable Methods

//static
void
UOdysseyBrushMathLibrary::ConvertPolarToCartesianCoordinates( float AngleRad, float  Radius, float& DeltaX, float& DeltaY )
{
    float cosa = cos( AngleRad );
    float sina = sin( AngleRad );
    DeltaX = cosa * Radius;
    DeltaY = sina * Radius;
}

//static
void
UOdysseyBrushMathLibrary::ConvertCartesianToPolarCoordinates( float DeltaX, float DeltaY, float& AngleRad, float& Radius )
{
    AngleRad = atan2(DeltaY, DeltaX);
    Radius = sqrt(DeltaY * DeltaY + DeltaX * DeltaX);
}

#undef LOCTEXT_NAMESPACE

