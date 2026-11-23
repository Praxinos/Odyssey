// IDDN.FR.001.060015.015.S.X.2019.000.00000
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
    /**
     * Import Texture 2D assets into the Animation 2D.
     *
     * Create a new Raster Layer in which each Texture 2D becomes a Cell.
     * Return the created Layer and Cells.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static FOdysseyPainterEditorAnimationImportResult ImportTextureSequence(UOdysseyAnimation* Animation, TArray<UTexture2D*> Textures, FOdysseyImportTexturesParameters Parameters);

    /**
     * Import an image sequence from disk into the Animation 2D.
     *
     * Create a new Raster Layer in which each image file becomes a Cell.
     * Return the created Layer and Cells.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static FOdysseyPainterEditorAnimationImportResult ImportImageSequence(UOdysseyAnimation* Animation, TArray<FString> Paths, FOdysseyImportTexturesParameters Parameters);

    /**
     * Export one frame of the Animation 2D as an image file on disk.
     *
     * Return the full path of the written file, or an empty string
     * on failure.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static FString ExportFrameAsImage(
        UOdysseyAnimation* Animation,
        int Frame,
        FString Filename,
        FString Path,
        EOdysseyExportImageFormat Format
    );

    /** Export one frame of the Animation 2D as a new Texture 2D asset. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UTexture2D* ExportFrameAsTexture(
        UOdysseyAnimation* Animation,
        int Frame,
        FString Filename,
        FString Path
    );

    /**
     * Export a range of frames of the Animation 2D as image files.
     *
     * One image is written per drawing change; frames identical to the
     * previous one are skipped. Return the paths of the written files.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static TArray<FString> ExportAsImageSequence(
        UOdysseyAnimation* Animation,
        FInt32Range FrameRange,
        FString Filename,
        FString Path,
        EOdysseyExportImageFormat Format
    );

    /**
     * Export a range of frames of the Animation 2D as Texture 2D assets.
     *
     * One texture is created per drawing change; frames identical to the
     * previous one are skipped.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static TArray<UTexture2D*> ExportAsTextureSequence(UOdysseyAnimation* Animation, FInt32Range FrameRange, FString AssetName, FString Path);

    /**
     * Convert a range of frames of the Animation 2D into a Flipbook.
     *
     * Create a Flipbook asset with individual Sprites and Texture 2D
     * assets, using the frame rate of the Animation 2D.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UPaperFlipbook* ExportAsFlipbook(UOdysseyAnimation* Animation, FInt32Range FrameRange, FString AssetName, FString Path);
};

UCLASS()
class UOdysseyPainterEditorAnimationLayerFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Import Texture 2D assets as Cells of this Raster Layer.
     *
     * Each Texture 2D becomes a new Cell inserted at Cell Index
     * (-1 appends after the last Cell). Return the created Cells.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static FOdysseyPainterEditorAnimationImportResult ImportTextureSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<UTexture2D*> Textures, FOdysseyImportTexturesParameters Parameters, int CellIndex = -1);

    /**
     * Import an image sequence from disk as Cells of this Raster Layer.
     *
     * Each image file becomes a new Cell inserted at Cell Index
     * (-1 appends after the last Cell). Return the created Cells.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static FOdysseyPainterEditorAnimationImportResult ImportImageSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<FString> Paths, FOdysseyImportTexturesParameters Parameters, int CellIndex = -1);

    /**
     * Export one frame of the Layer as an image file on disk.
     *
     * Return the full path of the written file, or an empty string
     * on failure.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static FString ExportFrameAsImage(
        UOdysseyAnimationLayer* Layer,
        int Frame,
        FString Filename,
        FString Path,
        EOdysseyExportImageFormat Format
    );

    /** Export one frame of the Layer as a new Texture 2D asset. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UTexture2D* ExportFrameAsTexture(
        UOdysseyAnimationLayer* Layer,
        int Frame,
        FString Filename,
        FString Path
    );

    /**
     * Export a range of frames of the Layer as image files.
     *
     * One image is written per drawing change; frames identical to the
     * previous one are skipped. Return the paths of the written files.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static TArray<FString> ExportAsImageSequence(
        UOdysseyAnimationLayer* Layer,
        FInt32Range FrameRange,
        FString Filename,
        FString Path,
        EOdysseyExportImageFormat Format
    );

    /**
     * Export a range of frames of the Layer as Texture 2D assets.
     *
     * One texture is created per drawing change; frames identical to the
     * previous one are skipped.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static TArray<UTexture2D*> ExportAsTextureSequence(UOdysseyAnimationLayer* Layer, FInt32Range FrameRange, FString AssetName, FString Path);

    /**
     * Convert a range of frames of the Layer into a Flipbook.
     *
     * Create a Flipbook asset with individual Sprites and Texture 2D
     * assets, using the frame rate of the Animation 2D.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Animation")
    static UPaperFlipbook* ExportAsFlipbook(UOdysseyAnimationLayer* Layer, FInt32Range FrameRange, FString AssetName, FString Path);
};

UCLASS()
class UOdysseyPainterEditorAnimationCellFunctionLibrary: public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Replace the drawing of this Cell with a Texture 2D asset. */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Animation" )
    static FOdysseyPainterEditorAnimationImportResult ImportTextureSequence( UOdysseyAnimationCellImageRaster* CellRaster, UTexture2D* Texture, FOdysseyImportTexturesParameters Parameters );

    /** Replace the drawing of this Cell with an image file from disk. */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Animation" )
    static FOdysseyPainterEditorAnimationImportResult ImportImageSequence( UOdysseyAnimationCellImageRaster* CellRaster, FString Path, FOdysseyImportTexturesParameters Parameters );
};
