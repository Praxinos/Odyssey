// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "Proxies/OdysseyBrushColor.h"
#include "Proxies/OdysseyBrushFormat.h"
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

    //Debug Stamp, stamps a basic shape with the current modifiers parameters. Is also pressure sensitive.
    UFUNCTION( BlueprintCallable
             , Category="OdysseyBrushStamps"
             , meta = ( DefaultToSelf="BrushContext" ) )
    static  void  DebugStamp( UOdysseyBrushAssetBase* BrushContext );

    //Simple version of Stamp, without Blend and Alpha options.
    UFUNCTION( BlueprintCallable
             , Category="OdysseyBrushStamps"
             , meta = ( DefaultToSelf="BrushContext", DeprecatedFunction, DeprecationMessage = "Use Stamp" ) )
    static  void  SimpleStamp( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow = 1.f, bool AntiAliasing = false );

    //Node that reunites all the necessary information to create a stamp.
    //Must be connected to an Event (i.e. "Event on Step").
    //Requires 3 mandatory input to work : Odyssey Block Reference and X|Y coordinates.
    UFUNCTION( BlueprintCallable
            , Category="OdysseyBrushStamps"
            , meta = ( DefaultToSelf="BrushContext" ) )
    static  void  Stamp( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow = 1.f, bool AntiAliasing = false, EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal, EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal );
};
