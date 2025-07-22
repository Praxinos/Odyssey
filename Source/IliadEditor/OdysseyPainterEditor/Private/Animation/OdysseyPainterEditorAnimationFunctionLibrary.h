// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "OdysseyAnimation.h"
#include "OdysseyPainterEditorAnimationFunctionLibrary.generated.h"

UCLASS()
class UOdysseyPainterEditorAnimationFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
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
class UOdysseyPainterEditorAnimationLayerFunctionLibrary : public UBlueprintFunctionLibrary
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
