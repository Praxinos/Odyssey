// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Proxies/OdysseyBrushBlock.h"
#include "Proxies/OdysseyBrushRect.h"

#include "OdysseyTextureEditorFunctionLibrary.generated.h"

class UOdysseyBrushAssetBase;

//---

UCLASS(Blueprintable)
class UOdysseyTextureEditorFunctionLibrary 
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    //Creates and Odyssey Block Reference with current layer.
    //X | Y input are an offset.
    /*UFUNCTION(BlueprintPure, Category = "Odyssey|Block")
    static FOdysseyBlockProxy GetBlockOfCurrentLayer( UOdysseyBrushAssetBase* BrushContext, FOdysseyBrushRect Area );
    
    //Creates and Odyssey Block Reference with a layer to be identified with its name (to be entered in a string).
    //X | Y input are an offset.
    UFUNCTION( BlueprintPure, Category = "Odyssey|Block" )
    static FOdysseyBlockProxy GetBlockOfLayerByName( UOdysseyBrushAssetBase* BrushContext, const FString& Name, FOdysseyBrushRect Area );
    
    //Creates and Odyssey Block Reference with a layer to be identified with an index, depending on the layer position in the layer stack (1 = top layer).
    //X | Y input are an offset.
    UFUNCTION( BlueprintPure, Category = "Odyssey|Block" )
    static FOdysseyBlockProxy GetBlockOfLayerByIndex( UOdysseyBrushAssetBase* BrushContext, int Index, FOdysseyBrushRect Area );
    
#if 0
    UFUNCTION( BlueprintPure, Category = "Odyssey|Block" )
    static FOdysseyBlockProxy GetResultBlock( UOdysseyBrushAssetBase* BrushContext );
#endif
	*/
};
