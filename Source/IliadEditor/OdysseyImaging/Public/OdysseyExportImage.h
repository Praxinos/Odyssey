// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

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
class UTexture2D;

namespace Odyssey
{
    ODYSSEYIMAGING_API UTexture2D* ExportAsTexture(UObject* iObject, const FOdysseyRenderContext* RenderContext, FString iAssetName, FString iPath);
    ODYSSEYIMAGING_API FString ExportAsImage(UObject* iObject, const FOdysseyRenderContext* RenderContext, EOdysseyExportImageFormat iFormat, FString iFilename, FString iPath);

    ODYSSEYIMAGING_API ::ULIS::eFileFormat GetFileExportImageFormat(EOdysseyExportImageFormat iFormat);
    ODYSSEYIMAGING_API FString GetFileFormatExtension(EOdysseyExportImageFormat iFormat);
};
