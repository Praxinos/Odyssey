// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "Proxies/OdysseyBrushBlock.h"
#include "Proxies/OdysseyBrushRect.h"

#include "OdysseyLayerStackBrushEditorFunctionLibrary.generated.h"

class UOdysseyBrushAssetBase;

//---

UCLASS(Blueprintable)
class UOdysseyLayerStackBrushEditorFunctionLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    //Creates and Odyssey Block Reference with current layer.
    //X | Y input are an offset.
    UFUNCTION(BlueprintPure, Category="Odyssey|Block")
    static FOdysseyBlockProxy GetBlockOfCurrentLayer( UOdysseyBrushAssetBase* BrushContext, FOdysseyBrushRect Area );

    //Creates and Odyssey Block Reference with a layer to be identified with its name (to be entered in a string).
    //X | Y input are an offset.
    UFUNCTION( BlueprintPure, Category="Odyssey|Block" )
    static FOdysseyBlockProxy GetBlockOfLayerByName( UOdysseyBrushAssetBase* BrushContext, const FString& Name, FOdysseyBrushRect Area );

    //Creates and Odyssey Block Reference with a layer to be identified with an index, depending on the layer position in the layer stack (1 = top layer).
    //X | Y input are an offset.
    UFUNCTION( BlueprintPure, Category="Odyssey|Block" )
    static FOdysseyBlockProxy GetBlockOfLayerByIndex( UOdysseyBrushAssetBase* BrushContext, int Index, FOdysseyBrushRect Area );
};
