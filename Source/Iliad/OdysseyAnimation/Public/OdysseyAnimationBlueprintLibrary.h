// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "OdysseyAnimationBlueprintLibrary.generated.h"

class UOdysseyAnimation;
class UOdysseyAnimationCellImageRaster;
class UOdysseyLayerStack;
class UOdysseyLayer;
class UOdysseyLayerCell;

UCLASS(BlueprintType)
class UOdysseyAnimationBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    UFUNCTION(BlueprintPure, Category = "Odyssey|LayerStack", meta=(DisplayName="Get Animation", HidePin="Layerstack", DefaultToSelf="Layerstack"))
    static UOdysseyAnimation* GetLayerStackAnimation(UOdysseyLayerStack* Layerstack);

    UFUNCTION(BlueprintPure, Category = "Odyssey|Layer", meta=(DisplayName="Get Animation", HidePin="Layer", DefaultToSelf="Layer"))
    static UOdysseyAnimation* GetLayerAnimation(UOdysseyLayer* Layer);

    UFUNCTION(BlueprintPure, Category = "Odyssey|Cell", meta=(DisplayName="Get Animation", HidePin="Cell", DefaultToSelf="Cell"))
    static UOdysseyAnimation* GetCellAnimation(UOdysseyLayerCell* Cell);

    /** Fill the cell content by a single color
      *
      * Warning: This function is temporary until a better way will be in place
      */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Cell")
    static void Fill( UOdysseyAnimationCellImageRaster* Cell, const FLinearColor& Color);
};
