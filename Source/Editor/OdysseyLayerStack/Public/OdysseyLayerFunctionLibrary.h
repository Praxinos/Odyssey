// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"
#include "OdysseyLayerFunctionLibrary.generated.h"

UCLASS()
class ODYSSEYLAYERSTACK_API UOdysseyLayerFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="LayerStack|Layer", meta = (HidePin="Layer", DefaultToSelf="Layer"))
    static bool IsLayerActivatedInStack(UOdysseyLayer* Layer);

    UFUNCTION(BlueprintPure, Category="LayerStack|Layer", meta = (HidePin="Layer", DefaultToSelf="Layer"))
    static bool IsLayerLockedInStack(UOdysseyLayer* Layer);
};