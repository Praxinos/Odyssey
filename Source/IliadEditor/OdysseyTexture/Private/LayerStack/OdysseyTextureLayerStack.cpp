// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerFolder.h"
#include "OdysseyRasterBlock.h"
#include "TextureCompiler.h"
#include "UObject/ObjectSaveContext.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyTextureLayer.h"
#include "OdysseyTextureLayerImageVector.h"
#include "ULISLoaderModule.h"
#include "OdysseyPixelFormat.h"
#include "UObject/Package.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "ULISUtils.h"
#include "OdysseyTextureLayerStackUserData.h"

UOdysseyTextureLayerStack*
UOdysseyTextureLayerStack::CreateEmptyFromTexture(UTexture2D* iTexture, UObject* iOuter)
{
    UOdysseyTextureLayerStack* layerStack = NewObject<UOdysseyTextureLayerStack>(iOuter, "LayerStack", RF_Public | RF_Transactional);
    layerStack->mInvalidTileMap = FOdysseyInvalidTileMap(64, iTexture->Source.GetSizeX(), iTexture->Source.GetSizeY());
    return layerStack;
}

UOdysseyTextureLayerStack*
UOdysseyTextureLayerStack::CreateFromTexture(UTexture2D* iTexture, UObject* iOuter)
{
    UOdysseyTextureLayerStack* layerStack = NewObject<UOdysseyTextureLayerStack>(iOuter, "LayerStack", RF_Public | RF_Transactional);
    layerStack->mInvalidTileMap = FOdysseyInvalidTileMap(64, iTexture->Source.GetSizeX(), iTexture->Source.GetSizeY());

    //Add first layer image
    UOdysseyTextureLayerImageRaster* layer = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CreateLayer(UOdysseyTextureLayerImageRaster::StaticClass()));

    //Set the layer as Current Layer
    layerStack->GetLayerRoot()->AddChild(layer);
    layerStack->SetCurrentLayer(layer);

    //Fill LayerImage with content of Texture
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = layer->GetRasterBlock();
    FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
    rasterBlockMutator.EditTilesFromRects(
        { ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) },
        [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<ULIS::FEvent>
        {
            FillOdysseyBlockFromUTextureData(iBlock.Get(), iTexture, iBlock->Format());
            return {};
        }
    );
    rasterBlockMutator.Commit();

    return layerStack;
}

UOdysseyTextureLayerStack*
UOdysseyTextureLayerStack::CreateWithEmptyVectorLayer(UTexture2D* iTexture, UObject* iOuter)
{
    UOdysseyTextureLayerStack* layerStack = NewObject<UOdysseyTextureLayerStack>(iOuter, "LayerStack", RF_Public | RF_Transactional);
    layerStack->mInvalidTileMap = FOdysseyInvalidTileMap(64, iTexture->Source.GetSizeX(), iTexture->Source.GetSizeY());

    //Add first layer image
    UOdysseyTextureLayerImageVector* layer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CreateLayer(UOdysseyTextureLayerImageVector::StaticClass()));

    //Set the layer as Current Layer
    layerStack->GetLayerRoot()->AddChild(layer);
    layerStack->SetCurrentLayer(layer);

    return layerStack;
}

UOdysseyTextureLayerStack::~UOdysseyTextureLayerStack()
{
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
}

UOdysseyTextureLayerStack::UOdysseyTextureLayerStack()
{
    SupportedLayerClasses.Add(UOdysseyTextureLayerFolder::StaticClass());
    SupportedLayerClasses.Add(UOdysseyTextureLayerImageRaster::StaticClass());
    SupportedLayerClasses.Add(UOdysseyTextureLayerImageVector::StaticClass());

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyTextureLayerStack::OnRenderingChanged);
}

void
UOdysseyTextureLayerStack::PostInitProperties()
{
    Super::PostInitProperties();
}

void
UOdysseyTextureLayerStack::PostLoad()
{
    Super::PostLoad();
    UTexture2D* texture = GetTexture();
    if (texture)
    {
        mInvalidTileMap = FOdysseyInvalidTileMap(64, texture->Source.GetSizeX(), texture->Source.GetSizeY());
    }
}

/* int
UOdysseyTextureLayerStack::GetWidth() const
{
    UTexture2D* texture = GetTexture();
    if (!texture)
        return Super::GetWidth();

    return texture->GetSizeX();
}

int
UOdysseyTextureLayerStack::GetHeight() const
{
    UTexture2D* texture = GetTexture();
    if (!texture)
        return Super::GetHeight();

    return texture->GetSizeY();
} */

UTexture2D*
UOdysseyTextureLayerStack::GetTexture() const
{
    UObject* outer = GetOuter();
    while(outer)
    {
        if (outer->GetClass() == UTexture2D::StaticClass())
            return Cast<UTexture2D>(outer);

        outer = outer->GetOuter();
    }

    return nullptr;
}

FIntRect
UOdysseyTextureLayerStack::GetDefaultRenderRect() const
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return FIntRect(0, 0, 0, 0);

    return FIntRect(0, 0, texture->Source.GetSizeX(), texture->Source.GetSizeY());
}

void
UOdysseyTextureLayerStack::OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyTextureLayerStack::OnRenderingChanged);
    const FGuid& eventId =  iEvent.GetId();
    TArray<FGuid> composition = GetRenderingComposition(EOdysseyRenderingType::Editor, 0);
    if (!composition.Contains(eventId))
        return;

    if (!iEvent.IsInteractive())
    {
        //PATCH BEGIN: because Unreal Undo does not make package dirty correctly
        MarkPackageDirty();
        //PATCH END:
    }

    if (iEvent.GetType() == FOdysseyRenderingChangedEvent::eEventType::kValueChange)
    {
        mInvalidTileMap.Invalidate(iEvent.GetRects());
    }
    else if (iEvent.GetType() == FOdysseyRenderingChangedEvent::eEventType::kCompositionChange)
    {
        mInvalidTileMap.Invalidate();
    }

    if (mTextureUpdateMode == EOdysseyTextureLayerStackTextureUpdateMode::Instantaneous)
        UpdateTexture();
}

TSharedPtr<FOdysseySurfaceTexture2DEditable>
UOdysseyTextureLayerStack::GetSurface() const
{
    return mTextureFastUpdateSurface;
}

void
UOdysseyTextureLayerStack::InactivateTextureFastUpdate()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FCoreUObjectDelegates::OnPreObjectPropertyChanged.RemoveAll(this);
    UPackage::PreSavePackageWithContextEvent.RemoveAll(this);
    UPackage::PackageSavedWithContextEvent.RemoveAll(this);
    CompressTexture();
    texture->UpdateResource();
    mTextureFastUpdateSurface = nullptr;
}

void
UOdysseyTextureLayerStack::ActivateTextureFastUpdate()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    mTextureFastUpdateSurface = MakeShared<FOdysseySurfaceTexture2DEditable>(texture);
    UncompressTexture();

    FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddUObject(this, &UOdysseyTextureLayerStack::OnPreGlobalObjectPropertyChanged);
    UPackage::PreSavePackageWithContextEvent.AddUObject(this, &UOdysseyTextureLayerStack::OnPackagePreSave);
    UPackage::PackageSavedWithContextEvent.AddUObject(this, &UOdysseyTextureLayerStack::OnPackageSaved);
}

void
UOdysseyTextureLayerStack::UncompressTexture()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FTextureFormatSettings textureFormatSettings;
    texture->GetLayerFormatSettings(0, textureFormatSettings);

    // Create new Texture Properties Backup
    mTextureCompressionNone = textureFormatSettings.CompressionNone;

    // Overwrite Texture properties
    textureFormatSettings.CompressionNone = 1;
    texture->SetLayerFormatSettings(0, textureFormatSettings);

    //needed because we need the texture resource to be uncompressed and ready for edition
    texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ texture });
    texture->TemporarilyDisableStreaming(); //needed to be able to draw on previously streamed textures, avoids using NoMipMaps
}

void
UOdysseyTextureLayerStack::CompressTexture()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FTextureFormatSettings textureFormatSettings;
    texture->GetLayerFormatSettings(0, textureFormatSettings);
    textureFormatSettings.CompressionNone = mTextureCompressionNone;
    texture->SetLayerFormatSettings(0, textureFormatSettings);
    UpdateTextureSource();
}

void
UOdysseyTextureLayerStack::FastUpdateTexture(const TArray<FIntRect>& iRects)
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FTextureCompilingManager::Get().FinishCompilation({ texture });
    ETextureSourceFormat textureSourceFormat = texture->Source.GetFormat();

    for (const FIntRect& rect : iRects)
    {
        Render_GameThread(RenderTarget, FFrameNumber(0), EOdysseyRenderingType::Editor, rect);
    }

    //Fence ?
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();

    TSharedPtr<::ULIS::FBlock> dst = mTextureFastUpdateSurface->Block();
    for (const FIntRect& rect : iRects)
    {
        FImage OutImage;
        if (!FImageUtils::GetRenderTargetImage(RenderTarget, OutImage, rect))
            continue;

        ::ULIS::eFormat srcFormat = ULISFormatForRawImageFormat( OutImage.Format );
        ::ULIS::FBlock src(OutImage.GetWidth(), OutImage.GetHeight(), srcFormat);
        CopyImageToBlock(OutImage, &src);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(dst->Format());
        ctx.ConvertFormat(src, *dst, src.Rect(), ::ULIS::FVec2I(rect.Min.X, rect.Min.Y));
        ctx.Finish();
    }

    mTextureFastUpdateSurface->Invalidate(::ULISUtils::ToULISRectIs(iRects));
}

void
UOdysseyTextureLayerStack::UpdateTextureSource()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    CopyBlockDataIntoUTexture(mTextureFastUpdateSurface->Block().Get(), texture);
}

void
UOdysseyTextureLayerStack::OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain)
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    if ( texture != Cast<UTexture2D>(iObject) )
        return;

    UpdateTextureSource();
    // Texture->UpdateResource();
    //FTextureCompilingManager::Get().FinishCompilation({Texture});
}

void
UOdysseyTextureLayerStack::OnPackagePreSave(UPackage* iPackage, FObjectPreSaveContext ObjectSaveContext)
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    UPackage* package = CastChecked<UPackage>(texture->GetOuter());
    if ( package != iPackage )
        return;

    CompressTexture();
}

void
UOdysseyTextureLayerStack::OnPackageSaved(const FString& iPackageFilename, UPackage* iPackage, FObjectPostSaveContext ObjectSaveContext)
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    UPackage* package = CastChecked<UPackage>(texture->GetOuter());
    if ( package != iPackage )
        return;

    UncompressTexture();
}

void
UOdysseyTextureLayerStack::SetTextureUpdateMode(EOdysseyTextureLayerStackTextureUpdateMode iMode)
{
    mTextureUpdateMode = iMode;
    if (mTextureUpdateMode == EOdysseyTextureLayerStackTextureUpdateMode::Instantaneous)
        UpdateTexture();
}

EOdysseyTextureLayerStackTextureUpdateMode
UOdysseyTextureLayerStack::GetTextureUpdateMode()
{
    return mTextureUpdateMode;
}

void
UOdysseyTextureLayerStack::UpdateTexture(bool iForceRefresh)
{
    if (iForceRefresh)
        mInvalidTileMap.Invalidate();

    if (mInvalidTileMap.InvalidTiles().IsEmpty())
        return;

    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    if (!RenderTarget)
        CreateRenderTarget();

    UpdateRenderTargetFormat();

    if ( mTextureFastUpdateSurface.IsValid() )
    {
        FastUpdateTexture(mInvalidTileMap.InvalidRects());
    }
    else
    {
        TArray<FIntRect> invalidRects = mInvalidTileMap.InvalidRects();

        for (const FIntRect& rect : invalidRects)
        {
            Render_GameThread(RenderTarget, FFrameNumber(0), EOdysseyRenderingType::Render, rect);
        }

        ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> dst = MakeShareable(NewBlockFromUTextureData(texture, format));

        //Fence ?
        for (const FIntRect& rect : invalidRects)
        {
            FImage OutImage;
            if (!FImageUtils::GetRenderTargetImage(RenderTarget, OutImage, rect))
                continue;

            ::ULIS::eFormat srcFormat = ULISFormatForRawImageFormat( OutImage.Format );
            ::ULIS::FBlock src(OutImage.GetWidth(), OutImage.GetHeight(), srcFormat);
            CopyImageToBlock(OutImage, &src);

            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(dst->Format());
            ctx.ConvertFormat(src, *dst, src.Rect(), ::ULIS::FVec2I(rect.Min.X, rect.Min.Y));
            ctx.Finish();
        }

        CopyBlockDataIntoUTexture(dst.Get(), texture);

        texture->UpdateResource();
    }

    mInvalidTileMap.Clear();
}

void
UOdysseyTextureLayerStack::Tick(float DeltaTime)
{
    if (mTextureUpdateMode != EOdysseyTextureLayerStackTextureUpdateMode::OnTick)
        return;

    UpdateTexture();
}

void
UOdysseyTextureLayerStack::CreateRenderTarget()
{
    RenderTarget = NewObject<UTextureRenderTarget2D>(this, NAME_None, RF_Public);
}

void
UOdysseyTextureLayerStack::UpdateRenderTargetFormat()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FTextureCompilingManager::Get().FinishCompilation({ texture });

    ETextureSourceFormat textureSourceFormat = texture->Source.GetFormat();
    //bool expectSRGB = false; //(textureSourceFormat == TSF_BGRA8 || textureSourceFormat == TSF_G8) && texture->SRGB;
    ETextureRenderTargetFormat format = RTF_RGBA8_SRGB;

    switch(textureSourceFormat)
    {
        case TSF_BGRA8:
        case TSF_BGRE8:
        case TSF_G8:
        case TSF_RGBA8_DEPRECATED:
        case TSF_RGBE8_DEPRECATED:
        {
            format = RTF_RGBA8; //expectSRGB ? RTF_RGBA8_SRGB : RTF_RGBA8;
        }
        break;

        case TSF_R16F: //we could use RTF_R16f here
        case TSF_G16:
        case TSF_RGBA16:
        case TSF_RGBA16F:
        {
            format = RTF_RGBA16f;
        }

        case TSF_R32F: //we could use RTF_R32f here
        case TSF_RGBA32F:
        {
            format = RTF_RGBA32f;
        }
    }


    if (RenderTarget->RenderTargetFormat != format)
    {
        RenderTarget->RenderTargetFormat = format;
        RenderTarget->bForceLinearGamma = true; //!expectSRGB;
        RenderTarget->PostEditChange();
        RenderTarget->UpdateResourceImmediate();
    }

    if (RenderTarget->SizeX != texture->GetSizeX() || RenderTarget->SizeY != texture->GetSizeY() )
    {
        RenderTarget->ResizeTarget(texture->GetSizeX(), texture->GetSizeY());
        RenderTarget->UpdateResourceImmediate();
    }
}

UTextureRenderTarget2D*
UOdysseyTextureLayerStack::CreateRenderingRenderTarget()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return nullptr;

    FTextureCompilingManager::Get().FinishCompilation({ texture });

    ETextureSourceFormat textureSourceFormat = texture->Source.GetFormat();
    ETextureRenderTargetFormat format = RTF_RGBA8_SRGB;

    switch(textureSourceFormat)
    {
        case TSF_BGRA8:
        case TSF_BGRE8:
        case TSF_G8:
        case TSF_RGBA8_DEPRECATED:
        case TSF_RGBE8_DEPRECATED:
        {
            format = RTF_RGBA8; //expectSRGB ? RTF_RGBA8_SRGB : RTF_RGBA8;
        }
        break;

        case TSF_R16F: //we could use RTF_R16f here
        case TSF_G16:
        case TSF_RGBA16:
        case TSF_RGBA16F:
        {
            format = RTF_RGBA16f;
        }

        case TSF_R32F: //we could use RTF_R32f here
        case TSF_RGBA32F:
        {
            format = RTF_RGBA32f;
        }
    }

    UTextureRenderTarget2D* renderTarget = NewObject<UTextureRenderTarget2D>();
    renderTarget->RenderTargetFormat = format;
    renderTarget->bForceLinearGamma = true;
    renderTarget->SRGB = renderTarget->IsSRGB();
    renderTarget->InitAutoFormat(texture->GetSizeX(), texture->GetSizeY());

    return renderTarget;
}

UTexture2D*
UOdysseyTextureLayerStack::CreateExportTexture(UObject* Outer, FName Name, EObjectFlags Flags)
{
    FObjectDuplicationParameters params(GetTexture(), Outer);
    params.DestName = Name;
    params.ApplyFlags = Flags;

    UTexture2D* texture = Cast<UTexture2D>(StaticDuplicateObjectEx(params));
    texture->RemoveUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass());

    return texture;
}
