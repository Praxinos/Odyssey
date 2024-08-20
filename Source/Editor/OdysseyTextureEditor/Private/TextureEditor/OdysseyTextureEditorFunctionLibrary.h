// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"
#include "OdysseyTextureEditorFunctionLibrary.generated.h"

UCLASS()
class UOdysseyTextureEditorFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static UTexture2D* CreateTextureAsset(FString AssetName, FString PackagePath, int Width, int Height, EOdysseyTextureSourceFormat Format);
};