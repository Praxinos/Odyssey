// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "Proxies/OdysseyBrushColor.h"
#include "Proxies/OdysseyBrushPivot.h"
#include "Proxies/OdysseyBrushBlock.h"
#include "OdysseyBrushFunctionLibrary.generated.h"

UCLASS(meta=(ScriptName="OdysseyBrushLibrary"))
class ODYSSEYBRUSH_API UOdysseyBrushFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Odyssey Brush Blueprint Callable Methods
    UFUNCTION( BlueprintCallable, Category="OdysseyBrushFunctionLibrary" )
    static  void  Stamp( FOdysseyBlockProxy Sample, const  FOdysseyBrushColor&  Color, float X, float Y, float Angle, float ScaleX = 1.f, float ScaleY = 1.f, float Flow = 1.f );

    /** Test of tooltip */
    UFUNCTION( BlueprintCallable, Category="OdysseyBrushFunctionLibrary" )
    static  void  DebugStamp();

    /**
     *  Test of
     *  Long ToolTip
     */
    UFUNCTION( BlueprintCallable, Category="OdysseyBrushFunctionLibrary" )
    static  void  SimpleStamp( FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow = 1.f );

    UFUNCTION( BlueprintCallable, Category="OdysseyBrushFunctionLibrary" )
    static  void  GenerateOrbitDelta( float AngleRad, float  Radius, float& DeltaX, float& DeltaY );
};
