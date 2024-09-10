// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimation.h"
#include "OdysseyAnimationEditorFunctionLibrary.generated.h"

UCLASS()
class UOdysseyAnimationEditorFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UOdysseyAnimation* CreateAnimationAsset(FString AssetName="Animation", FString PackagePath="/Game/", int Width=1920, int Height=1080, EOdysseyAnimationFormat Format=EOdysseyAnimationFormat::BGRA8, float FramesPerSecond=24.f);

	UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
	static void ImportTextureSequence(UOdysseyAnimation* Animation, TArray<UTexture2D*> Textures);

	UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
	static void ImportImageSequence(UOdysseyAnimation* Animation, TArray<FString> Paths);

	UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
	static void ExportImageSequence(
		UOdysseyAnimation* Animation,
		FString Path,
		EOdysseyAnimationExportImageSequenceFormat Format,
		EOdysseyAnimationExportImageSequenceSource Source,
		EOdysseyAnimationExportImageSequenceRange Range,
		FInt32Range CustomRange,
		bool UniqueFramesOnly
	);

	UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
	static void ExportAsFlipbook(UOdysseyAnimation* Animation, FString AssetName, FString Path);
};