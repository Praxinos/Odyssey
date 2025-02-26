// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static UTexture2D* CreateTextureAsset(FString AssetName="Texture", FString PackagePath="/Game/", int Width=1024, int Height=1024, EOdysseyTextureSourceFormat Format=EOdysseyTextureSourceFormat::kBGRA8);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static UOdysseyTextureLayerImageRaster* ImportTexture(UTexture2D* Texture, UTexture2D* TextureToImport , UOdysseyTextureLayer* ParentLayer = nullptr, int IndexInLayer = -1);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static UOdysseyTextureLayerImageRaster* ImportImage(UTexture2D* Texture, FString Path, UOdysseyTextureLayer* ParentLayer = nullptr, int IndexInLayer = -1);

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
    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static FString ExportAsImage(
        UOdysseyTextureLayer* Layer,
        FString Filename,
        FString Path,
        EOdysseyExportImageFormat Format
    );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Texture")
    static UTexture2D* ExportAsTexture(
        UOdysseyTextureLayer* Layer,
        FString Filename,
        FString Path
    );
};
