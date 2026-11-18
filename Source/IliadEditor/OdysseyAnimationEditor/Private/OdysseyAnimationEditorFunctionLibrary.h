// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationEditorFunctionLibrary.generated.h"

class UOdysseyLayerStack;
class UOdysseyLayer;
class UOdysseyAnimationCell;

UCLASS()
class UOdysseyAnimationEditorFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UOdysseyAnimation* CreateAnimationAsset(FString AssetName="Animation", FString PackagePath="/Game/", int Width=1920, int Height=1080, EOdysseyAnimationFormat Format=EOdysseyAnimationFormat::BGRA8, float FramesPerSecond=24.f, TSubclassOf<UOdysseyAnimationLayer> DefaultLayerClass=nullptr);

    UFUNCTION(BlueprintPure, Category = "Odyssey|LayerStack", meta=(DisplayName="Get Animation", HidePin="Layerstack", DefaultToSelf="Layerstack"))
    static UOdysseyAnimation* GetLayerStackAnimation(UOdysseyLayerStack* Layerstack);

    UFUNCTION(BlueprintPure, Category = "Odyssey|Layer", meta=(DisplayName="Get Animation", HidePin="Layer", DefaultToSelf="Layer"))
    static UOdysseyAnimation* GetLayerAnimation(UOdysseyLayer* Layer);

    UFUNCTION(BlueprintPure, Category = "Odyssey|Cell", meta=(DisplayName="Get Animation", HidePin="Cell", DefaultToSelf="Cell"))
    static UOdysseyAnimation* GetCellAnimation(UOdysseyAnimationCell* Cell);
};
