// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationEditorFunctionLibrary.generated.h"

UCLASS()
class UOdysseyAnimationEditorAnimationFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UOdysseyAnimation* CreateAnimationAsset(FString AssetName="Animation", FString PackagePath="/Game/", int Width=1920, int Height=1080, EOdysseyAnimationFormat Format=EOdysseyAnimationFormat::BGRA8, float FramesPerSecond=24.f);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UOdysseyAnimationLayerImageRaster* ImportTextureSequence(UOdysseyAnimation* Animation, TArray<UTexture2D*> Textures, UOdysseyAnimationLayer* ParentLayer = nullptr, int IndexInParent = 0);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UOdysseyAnimationLayerImageRaster* ImportImageSequence(UOdysseyAnimation* Animation, TArray<FString> Paths, UOdysseyAnimationLayer* ParentLayer = nullptr, int IndexInParent = 0);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static FString ExportFrameAsImage(
        UOdysseyAnimation* Animation,
        int Frame,
        FString Filename,
        FString Path,
        EOdysseyExportImageFormat Format
    );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UTexture2D* ExportFrameAsTexture(
        UOdysseyAnimation* Animation,
        int Frame,
        FString Filename,
        FString Path
    );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static TArray<FString> ExportAsImageSequence(
        UOdysseyAnimation* Animation,
        FInt32Range FrameRange,
        FString Filename,
        FString Path,
        EOdysseyExportImageFormat Format
    );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static TArray<UTexture2D*> ExportAsTextureSequence(UOdysseyAnimation* Animation, FInt32Range FrameRange, FString AssetName, FString Path);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UPaperFlipbook* ExportAsFlipbook(UOdysseyAnimation* Animation, FInt32Range FrameRange, FString AssetName, FString Path);
};

UCLASS()
class UOdysseyAnimationEditorLayerFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static TArray<UOdysseyAnimationCellImageRaster*> ImportTextureSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<UTexture2D*> Textures, int iCellIndex = -1);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static TArray<UOdysseyAnimationCellImageRaster*> ImportImageSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<FString> Paths, int iCellIndex = -1);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static FString ExportFrameAsImage(
        UOdysseyAnimationLayer* Layer,
        int Frame,
        FString Filename,
        FString Path,
        EOdysseyExportImageFormat Format
    );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UTexture2D* ExportFrameAsTexture(
        UOdysseyAnimationLayer* Layer,
        int Frame,
        FString Filename,
        FString Path
    );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static TArray<FString> ExportAsImageSequence(
        UOdysseyAnimationLayer* Layer,
        FInt32Range FrameRange,
        FString Filename,
        FString Path,
        EOdysseyExportImageFormat Format
    );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static TArray<UTexture2D*> ExportAsTextureSequence(UOdysseyAnimationLayer* Layer, FInt32Range FrameRange, FString AssetName, FString Path);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UPaperFlipbook* ExportAsFlipbook(UOdysseyAnimationLayer* Layer, FInt32Range FrameRange, FString AssetName, FString Path);
};
