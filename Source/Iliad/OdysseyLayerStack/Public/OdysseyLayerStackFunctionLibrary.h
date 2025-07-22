// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyLayer.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "OdysseyLayerStackFunctionLibrary.generated.h"

UCLASS()
class ODYSSEYLAYERSTACK_API UOdysseyLayerStackFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    static TArray<UOdysseyLayer*> FilterTopmostLayers(TArray<UOdysseyLayer*> Layers);

    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    static TArray<UOdysseyLayer*> SortLayers(TArray<UOdysseyLayer*> Layers, bool Reverse);
};
