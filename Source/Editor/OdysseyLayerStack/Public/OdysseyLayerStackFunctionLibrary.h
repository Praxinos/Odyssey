// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"
#include "OdysseyLayerStackFunctionLibrary.generated.h"

UCLASS()
class ODYSSEYLAYERSTACK_API UOdysseyLayerStackFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="LayerStack")
    static TArray<UOdysseyLayer*> FilterTopmostLayers(TArray<UOdysseyLayer*> Layers);

    UFUNCTION(BlueprintCallable, Category="LayerStack")
    static TArray<UOdysseyLayer*> SortLayers(TArray<UOdysseyLayer*> iLayers, bool iReverse);
};