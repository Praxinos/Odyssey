// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyTextureRenderingAbility.h"
#include <ULIS>
#include "OdysseyExportImage.generated.h"

UENUM(BlueprintType)
enum class EOdysseyExportImageFormat : uint8
{
    PNG,
    BMP,
    TGA,
    Jpeg
};

class UPaperFlipbook;

namespace Odyssey
{
    ODYSSEYIMAGING_API UTexture2D* ExportAsTexture(UObject* iObject, int iFrame, const FIntRect& iRect, FString iAssetName, FString iPath);
    ODYSSEYIMAGING_API FString ExportAsImage(UObject* iObject, int iFrame, EOdysseyExportImageFormat iFormat, const FIntRect& iRect, FString iFilename, FString iPath);
    ODYSSEYIMAGING_API UPaperFlipbook* ExportAsFlipbook(UObject* iObject, const FInt32Range& iRange, const FIntRect& iRect, float iFramesPerSecond, FString AssetName, FString Path);
    ODYSSEYIMAGING_API TArray<UTexture2D*> ExportAsTextureSequence(UObject* iObject, const FInt32Range& iRange, const FIntRect& iRect, FString AssetName, FString Path);
    ODYSSEYIMAGING_API TArray<FString> ExportAsImageSequence(UObject* iObject, const FInt32Range& iRange, const FIntRect& iRect, FString Filename, FString Path, EOdysseyExportImageFormat Format);

    ODYSSEYIMAGING_API ::ULIS::eFileFormat GetFileExportImageFormat(EOdysseyExportImageFormat iFormat);
    ODYSSEYIMAGING_API FString GetFileFormatExtension(EOdysseyExportImageFormat iFormat);
};
