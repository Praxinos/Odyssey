// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
