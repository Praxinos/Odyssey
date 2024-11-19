// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyExportImageFormat.h"

::ULIS::eFileFormat
FOdysseyExportImageFormat::GetFileFormat(EOdysseyExportImageFormat iFormat)
{
    switch(iFormat)
    {
        case EOdysseyExportImageFormat::PNG: return ::ULIS::FileFormat_png;
        case EOdysseyExportImageFormat::BMP: return ::ULIS::FileFormat_bmp;
        case EOdysseyExportImageFormat::TGA: return ::ULIS::FileFormat_tga;
        case EOdysseyExportImageFormat::Jpeg: return ::ULIS::FileFormat_jpg;
    }

    return ::ULIS::FileFormat_png;
}

FString
FOdysseyExportImageFormat::GetFileFormatExtension(EOdysseyExportImageFormat iFormat)
{
    switch(iFormat)
    {
        case EOdysseyExportImageFormat::PNG: return TEXT("png");
        case EOdysseyExportImageFormat::BMP: return TEXT("bmp");
        case EOdysseyExportImageFormat::TGA: return TEXT("tga");
        case EOdysseyExportImageFormat::Jpeg: return TEXT("jpg");
    }

    check(false); //should not be called
    return TEXT("");
}
