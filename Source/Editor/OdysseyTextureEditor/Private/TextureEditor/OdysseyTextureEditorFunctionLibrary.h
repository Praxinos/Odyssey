// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyTextureConfigureWindow.h"
#include "OdysseyTextureEditorFunctionLibrary.generated.h"

class UOdysseyTextureFactory;

UCLASS()
class UOdysseyTextureEditorFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
	static UOdysseyTextureFactory* GetTextureFactory();

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static UTexture2D* CreateTextureAsset(FString AssetName="Texture", FString PackagePath="/Game/", int Width=1024, int Height=1024, EOdysseyTextureSourceFormat Format=EOdysseyTextureSourceFormat::kBGRA8);
};