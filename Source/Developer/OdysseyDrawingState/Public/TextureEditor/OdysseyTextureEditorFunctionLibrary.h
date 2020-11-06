// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "Proxies/OdysseyBrushBlock.h"
#include "Proxies/OdysseyBrushRect.h"

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
    UFUNCTION( BlueprintPure, Category = "Odyssey|Block", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetBlockOfCurrentLayer( UOdysseyBrushAssetBase* BrushContext, FOdysseyBrushRect Area );
    
    //Creates and Odyssey Block Reference with a layer to be identified with its name (to be entered in a string).
    //X | Y input are an offset.
    UFUNCTION( BlueprintPure, Category = "Odyssey|Block", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetBlockOfLayerByName( UOdysseyBrushAssetBase* BrushContext, const FString& Name, FOdysseyBrushRect Area );
    
    //Creates and Odyssey Block Reference with a layer to be identified with an index, depending on the layer position in the layer stack (1 = top layer).
    //X | Y input are an offset.
    UFUNCTION( BlueprintPure, Category = "Odyssey|Block", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetBlockOfLayerByIndex( UOdysseyBrushAssetBase* BrushContext, int Index, FOdysseyBrushRect Area );
    
#if 0
    UFUNCTION( BlueprintPure, Category = "Odyssey|Block", meta = (DefaultToSelf="BrushContext") )
    static FOdysseyBlockProxy GetResultBlock( UOdysseyBrushAssetBase* BrushContext );
#endif
};
