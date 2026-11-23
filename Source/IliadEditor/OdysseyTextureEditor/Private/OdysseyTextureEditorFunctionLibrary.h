// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Texture/SOdysseyTextureConfigureWindow.h"

#include "OdysseyTextureEditorFunctionLibrary.generated.h"

class UTexture2D;
class UOdysseyTextureLayer;
class UOdysseyTextureLayerImageRaster;
class UOdysseyTextureFactory;

UCLASS()
class UOdysseyTextureEditorTextureFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static UOdysseyTextureFactory* GetTextureFactory();

public:
    /**
     * Create a new Texture 2D asset that can be painted with Odyssey.
     *
     * The asset is created in Package Path with the given name,
     * dimensions and format.
     * Return null on invalid parameters.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static UTexture2D* CreateTextureAsset(FString AssetName="Texture", FString PackagePath="/Game/", int Width=1024, int Height=1024, EOdysseyTextureSourceFormat Format=EOdysseyTextureSourceFormat::kBGRA8);

    /**
     * Import a Texture 2D asset as a new Layer of the Texture 2D.
     *
     * The Layer is added under Parent Layer (the root if none) at
     * Index In Layer (-1 adds it as the last child).
     * Return the new Layer.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static UOdysseyTextureLayerImageRaster* ImportTexture(UTexture2D* Texture, UTexture2D* TextureToImport , UOdysseyTextureLayer* ParentLayer = nullptr, int IndexInLayer = -1);

    /**
     * Import an image file from disk as a new Layer of the Texture 2D.
     *
     * The Layer is added under Parent Layer (the root if none) at
     * Index In Layer (-1 adds it as the last child).
     * Return the new Layer.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static UOdysseyTextureLayerImageRaster* ImportImage(UTexture2D* Texture, FString Path, UOdysseyTextureLayer* ParentLayer = nullptr, int IndexInLayer = -1);

    /**
     * Export the Texture 2D as an image file on disk.
     *
     * Return the full path of the written file, or an empty string
     * on failure.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static FString ExportAsImage(
        UTexture2D* Texture,
        FString Filename,
        FString Path,
        EOdysseyExportImageFormat Format
    );
};



UCLASS()
class UOdysseyTextureEditorLayerFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Export the Layer as an image file on disk.
     *
     * Return the full path of the written file, or an empty string
     * on failure.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static FString ExportAsImage(
        UOdysseyTextureLayer* Layer,
        FString Filename,
        FString Path,
        EOdysseyExportImageFormat Format
    );

    /** Export the Layer as a new Texture 2D asset. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static UTexture2D* ExportAsTexture(
        UOdysseyTextureLayer* Layer,
        FString Filename,
        FString Path
    );
};
