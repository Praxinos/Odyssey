// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

#include "Proxies/OdysseyBrushBlock.h"
#include "Proxies/OdysseyBrushCache.h"

#include "OdysseyTextureEditorFunctionLibrary.generated.h"

class UOdysseyBrushAssetBase;

//---

UCLASS(meta=(ScriptName="OdysseyTextureEditorLibrary"))
class ODYSSEYDRAWINGSTATE_API UOdysseyTextureEditorFunctionLibrary 
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION( BlueprintPure, Category = "OdysseyTextureEditorFunctionLibrary", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetBlockOfCurrentLayer( UOdysseyBrushAssetBase* BrushContext, int X, int Y, int Width, int Height, ECacheLevel Cache );
    
    UFUNCTION( BlueprintPure, Category = "OdysseyTextureEditorFunctionLibrary", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetBlockOfLayerByName( UOdysseyBrushAssetBase* BrushContext, const FString& Name, int X, int Y, int Width, int Height, ECacheLevel Cache );
    
    UFUNCTION( BlueprintPure, Category = "OdysseyTextureEditorFunctionLibrary", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetBlockOfLayerByIndex( UOdysseyBrushAssetBase* BrushContext, int Index, int X, int Y, int Width, int Height, ECacheLevel Cache );
    
#if 0
    UFUNCTION( BlueprintPure, Category = "OdysseyTextureEditorFunctionLibrary", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetResultBlock( UOdysseyBrushAssetBase* BrushContext );
#endif
};
