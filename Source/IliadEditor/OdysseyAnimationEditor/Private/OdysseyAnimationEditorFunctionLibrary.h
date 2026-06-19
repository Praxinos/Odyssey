// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationEditorFunctionLibrary.generated.h"

UCLASS()
class UOdysseyAnimationEditorFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UOdysseyAnimation* CreateAnimationAsset(FString AssetName="Animation", FString PackagePath="/Game/", int Width=1920, int Height=1080, EOdysseyAnimationFormat Format=EOdysseyAnimationFormat::BGRA8, float FramesPerSecond=24.f, TSubclassOf<UOdysseyAnimationLayer> DefaultLayerClass=nullptr);
};
