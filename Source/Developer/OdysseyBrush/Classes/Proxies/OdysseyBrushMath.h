// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyBrushMath.generated.h"

UCLASS(meta=(ScriptName="OdysseyBrushMathLibrary"))
class ODYSSEYBRUSH_API UOdysseyBrushMathLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    //Node that converts polar coordinates (radius,angle) to cartesian coordinates (x,y).
    UFUNCTION( BlueprintPure, Category="Odyssey|Math" )
    static  void  ConvertPolarToCartesianCoordinates( float AngleRad, float  Radius, float& DeltaX, float& DeltaY );

    //Node that converts cartesian coordinates (x,y) to polar coordinates (radius,angle).
    UFUNCTION( BlueprintPure, Category="Odyssey|Math" )
    static  void  ConvertCartesianToPolarCoordinates( float DeltaX, float DeltaY, float& AngleRad, float& Radius );
};
