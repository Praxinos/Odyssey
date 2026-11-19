// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayer.h"
#include "OdysseyRenderContext.h"

#include "AssetToolsModule.h"
#include "ScreenPass.h"
#include "TextureCompiler.h"
#include "TextureResource.h"

#include "OdysseyBlendShader.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTexture2DUtils.h"

UTexture2D*
UOdysseyTextureLayer::GetTexture() const
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if(!layerStack)
        return nullptr;

    return layerStack->GetTexture();
}

void
UOdysseyTextureLayer::ColorSettingsChanged()
{
}

EOdysseyColorConversion
UOdysseyTextureLayer::GetContentColorConversion(const FOdysseyRenderContext* RenderContext) const
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    return layerStack ? layerStack->GetLayerColorConversion() : EOdysseyColorConversion::None;
}

ETextureSourceFormat
UOdysseyTextureLayer::GetLayerTextureFormat(ETextureSourceFormat TextureFormat)
{
    switch (TextureFormat)
    {
        case TSF_G16:
        case TSF_RGBA16:
            return TSF_RGBA16;

        case TSF_RGBA16F:
        case TSF_RGBA32F:
        case TSF_R16F:
        case TSF_R32F:
            return TSF_RGBA32F;

        default:
            return TSF_BGRA8;
    }
}

#if WITH_EDITOR
FSimpleMulticastDelegate&
UOdysseyTextureLayer::OnThumbnailChanged()
{
    return mOnThumbnailChanged;
}

FSimpleMulticastDelegate&
UOdysseyTextureLayer::OnThumbnailDirtied()
{
    return mOnThumbnailDirtied;
}

/* UTexture2D*
UOdysseyTextureLayer::ExportAsTexture(FString iAssetName, FString iPath )
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return nullptr;

    UTexture2D* renderTexture = GetRenderTexture();
    if ( !renderTexture )
        return nullptr;

    FString AssetName;
    FString PackageName;

    IAssetTools& AssetTools = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    AssetTools.CreateUniqueAssetName(iPath, iAssetName, PackageName, AssetName);

    FObjectDuplicationParameters params( texture, CreatePackage(*PackageName) );
    params.DestName = FName(*AssetName);
    params.FlagMask = RF_Public | RF_Standalone | RF_Transactional;
    params.DuplicateMode = EDuplicateMode::Normal;

    UTexture2D* createdTexture = Cast<UTexture2D>(StaticDuplicateObjectEx(params));


    FImage srcImage;
    renderTexture->Source.GetMipImage(srcImage, 0);

    createdTexture->PreEditChange(nullptr);
    createdTexture->Source.Init(srcImage);
    createdTexture->PostEditChange();

    return createdTexture;
} */

#endif
