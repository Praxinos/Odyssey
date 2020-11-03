// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

#include "Proxies/OdysseyBrushBlock.h"

#include "OdysseyTextureEditorFunctionLibrary.generated.h"

class UOdysseyBrushAssetBase;

//---

UCLASS(meta=(ScriptName="OdysseyTextureEditorLibrary"))
class ODYSSEYDRAWINGSTATE_API UOdysseyTextureEditorFunctionLibrary 
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    //Creates and Odyssey Block Reference with current layer.
    //X | Y input are an offset.
    UFUNCTION( BlueprintPure, Category = "OdysseyBlockReference", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetBlockOfCurrentLayer( UOdysseyBrushAssetBase* BrushContext, int X, int Y, int Width, int Height );
    
    //Creates and Odyssey Block Reference with a layer to be identified with its name (to be entered in a string).
    //X | Y input are an offset.
    UFUNCTION( BlueprintPure, Category = "OdysseyBlockReference", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetBlockOfLayerByName( UOdysseyBrushAssetBase* BrushContext, const FString& Name, int X, int Y, int Width, int Height );
    
    //Creates and Odyssey Block Reference with a layer to be identified with an index, depending on the layer position in the layer stack (1 = top layer).
    //X | Y input are an offset.
    UFUNCTION( BlueprintPure, Category = "OdysseyBlockReference", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetBlockOfLayerByIndex( UOdysseyBrushAssetBase* BrushContext, int Index, int X, int Y, int Width, int Height );
    
#if 0
    UFUNCTION( BlueprintPure, Category = "OdysseyBlockReference", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetResultBlock( UOdysseyBrushAssetBase* BrushContext );
#endif
};
