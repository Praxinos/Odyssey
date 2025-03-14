// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include <ULIS>

#include "OdysseyExportImageFormat.generated.h"

UENUM(BlueprintType)
enum class EOdysseyExportImageFormat : uint8
{
    PNG,
    BMP,
    TGA,
    Jpeg
};

class ODYSSEYIMAGING_API FOdysseyExportImageFormat
{
public:
    static ::ULIS::eFileFormat GetFileFormat(EOdysseyExportImageFormat iFormat);
    static FString GetFileFormatExtension(EOdysseyExportImageFormat iFormat);
};
