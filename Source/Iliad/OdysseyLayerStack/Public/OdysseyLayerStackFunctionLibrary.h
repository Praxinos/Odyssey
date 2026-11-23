// IDDN.FR.001.060015.015.S.X.2019.000.00000
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
    /**
     * Keep only the topmost layers of the given list.
     *
     * Layers whose parent is also in the list are removed.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    static TArray<UOdysseyLayer*> FilterTopmostLayers(TArray<UOdysseyLayer*> Layers);

    /**
     * Sort the given layers by their position in the stack.
     *
     * Parents come before children, then layers are sorted by index
     * in parent. Reverse inverts the order. All must share a stack.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    static TArray<UOdysseyLayer*> SortLayers(TArray<UOdysseyLayer*> Layers, bool Reverse);
};
