// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "Proxies/OdysseyBrushColor.h"
#include "Proxies/OdysseyBrushPivot.h"
#include "Proxies/OdysseyBrushBlock.h"
#include "Proxies/OdysseyBrushBlending.h"
#include "OdysseyBrushFunctionLibrary.generated.h"

UCLASS(meta=(ScriptName="OdysseyBrushLibrary"))
class ODYSSEYBRUSH_API UOdysseyBrushFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Odyssey Brush Blueprint Callable Methods

    /** Test of tooltip */
    UFUNCTION( BlueprintCallable, Category="OdysseyBrushFunctionLibrary" )
    static  void  DebugStamp();

    /** Simple Version of Stamp */
    UFUNCTION( BlueprintCallable, Category="OdysseyBrushFunctionLibrary" )
    static  void  SimpleStamp( FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow = 1.f );

    /** Stamp */
    UFUNCTION( BlueprintCallable, Category="OdysseyBrushFunctionLibrary" )
    static  void  Stamp( FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow = 1.f, EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal, EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal );


    UFUNCTION( BlueprintCallable, Category="OdysseyBrushFunctionLibrary" )
    static  void  GenerateOrbitDelta( float AngleRad, float  Radius, float& DeltaX, float& DeltaY );
};
