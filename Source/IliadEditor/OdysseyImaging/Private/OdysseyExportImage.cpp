// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyExportImage.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Factories/Texture2dFactoryNew.h"
#include "HAL/PlatformFileManager.h"
#include "ImageUtils.h"
#include "Misc/Paths.h"

#include "OdysseyRenderContext.h"

namespace Odyssey
{

::ULIS::eFileFormat
GetFileExportImageFormat(EOdysseyExportImageFormat iFormat)
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
GetFileFormatExtension(EOdysseyExportImageFormat iFormat)
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

UTexture2D*
ExportAsTexture(UObject* iObject, const FOdysseyRenderContext* RenderContext, FString iAssetName, FString iPath )
{
    if (!iObject->Implements<UOdysseyTextureRenderingAbility>())
        return nullptr;

    //Render in a RenderTarget
    IOdysseyTextureRenderingAbility* ability = Cast<IOdysseyTextureRenderingAbility>(iObject);
    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(ability->CreateRenderingRenderTarget());
    ability->Render_GameThread(renderTarget.Get(), EOdysseyRenderingType::Render, RenderContext);

    FImage OutImage;
    if (!FImageUtils::GetRenderTargetImage(renderTarget.Get(), OutImage))
        return nullptr;

    //Create the texture and update it with the renderTarget content
    UTexture2D* texture = ability->CreateExportTexture( iAssetName, iPath, UTexture2D::StaticClass(), UTexture2DFactoryNew::StaticClass()->GetDefaultObject<UFactory>() );
    check( texture );

    texture->PreEditChange(nullptr);
    texture->Source.Init(OutImage);
    texture->PostEditChange();

    return texture;
}

FString
ExportAsImage(UObject* iObject, const FOdysseyRenderContext* RenderContext, EOdysseyExportImageFormat iFormat, FString iFilename, FString iPath)
{
    if (!iObject->Implements<UOdysseyTextureRenderingAbility>())
        return "";

    IOdysseyTextureRenderingAbility* ability = Cast<IOdysseyTextureRenderingAbility>(iObject);
    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(ability->CreateRenderingRenderTarget());
    ability->Render_GameThread(renderTarget.Get(), EOdysseyRenderingType::Render, RenderContext);

    FImage OutImage;
    if (!FImageUtils::GetRenderTargetImage(renderTarget.Get(), OutImage))
        return "";

    FString extension = GetFileFormatExtension(iFormat);

    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();

    // Ensure Directory Exists
    if (!platformFile.DirectoryExists(*iPath))
        platformFile.CreateDirectory(*iPath);

    //Path
    FString imagePath = iPath / FPaths::GetBaseFilename(iFilename) + TEXT(".") + extension;

    if (!FImageUtils::SaveImageByExtension(*imagePath, OutImage))
        return "";

    return imagePath;
}


}
