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
#include "OdysseyBrushAssetBase.h"
#include "OdysseyBrushFunctionLibrary.generated.h"

UCLASS(meta=(ScriptName="OdysseyBrushLibrary"))
class ODYSSEYBRUSH_API UOdysseyBrushFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Odyssey Brush Blueprint Callable Methods

    /** Debug Stamp, stamps a basic shape with the current modifiers parameters. */
    UFUNCTION( BlueprintCallable
             , Category="OdysseyBrushFunctionLibrary"
             , meta = ( DefaultToSelf="BrushContext" ) )
    static  void  DebugStamp( UOdysseyBrushAssetBase* BrushContext );

    /** Simple Stamp ( may be faster than alternatives ). */
    UFUNCTION( BlueprintCallable
             , Category="OdysseyBrushFunctionLibrary"
             , meta = ( DefaultToSelf="BrushContext" ) )
    static  void  SimpleStamp( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow = 1.f );

    /** Deprecated Stamp */
    UFUNCTION( BlueprintCallable
            , Category="OdysseyBrushFunctionLibrary"
            , meta = ( DefaultToSelf="BrushContext" ) )
    static  void  Stamp( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow = 1.f, EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal, EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal );


    UFUNCTION( BlueprintCallable, Category="OdysseyBrushFunctionLibrary" )
    static  void  GenerateOrbitDelta( float AngleRad, float  Radius, float& DeltaX, float& DeltaY );
};
