// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerStack.h"

#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "TextureCompiler.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/Package.h"

#include "OdysseyPixelFormat.h"
#include "OdysseyRenderContext.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyTextureLayer.h"
#include "OdysseyTextureLayerFolder.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerImageVector.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyRasterContentBlocks.h"

#include "ULISLoaderModule.h"
#include "ULISUtils.h"

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
    Odyssey::RasterContentBlocks::EditBlock(layer->GetRasterContent(),
        [&](::ULIS::FBlock& iBlock)
        {
            FillOdysseyBlockFromUTextureData(&iBlock, iTexture, iBlock.Format());
        }
    );

    return layerStack;
}

UOdysseyTextureLayerStack*
UOdysseyTextureLayerStack::CreateWithEmptyRasterLayer(UTexture2D* iTexture, UObject* iOuter)
{
    UOdysseyTextureLayerStack* layerStack = NewObject<UOdysseyTextureLayerStack>(iOuter, "LayerStack", RF_Public | RF_Transactional);
    layerStack->mInvalidTileMap = FOdysseyInvalidTileMap(64, iTexture->Source.GetSizeX(), iTexture->Source.GetSizeY());

    //Add first layer image
    UOdysseyTextureLayerImageRaster* layer = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CreateLayer(UOdysseyTextureLayerImageRaster::StaticClass()));

    //Set the layer as Current Layer
    layerStack->GetLayerRoot()->AddChild(layer);
    layerStack->SetCurrentLayer(layer);

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

namespace
{
/** Rounds a float image to 8 bits when it is read into an 8-bit block, which a raw conversion would truncate. */
void RoundFloatImageForBlock(FImage& Image, ::ULIS::eFormat BlockFormat)
{
    const bool bFloatImage = Image.Format == ERawImageFormat::RGBA16F || Image.Format == ERawImageFormat::RGBA32F;
    if (!bFloatImage || ULIS_R_FLOATING(BlockFormat) || ULIS_R_DEPTH(BlockFormat) != 1)
        return;

    // The values are the bytes to write, already encoded when needed: no gamma conversion.
    FImage RoundedImage;
    Image.CopyTo(RoundedImage, ERawImageFormat::BGRA8, EGammaSpace::Linear);
    Image = MoveTemp(RoundedImage);
}

/** Reads an area of the render target back into the same area of a block, converted to its format. Waits for the GPU. */
void ReadBackRect(UTextureRenderTarget2D* RenderTarget, const FIntRect& Rect, ::ULIS::FBlock& Destination)
{
    FImage OutImage;
    if (!FImageUtils::GetRenderTargetImage(RenderTarget, OutImage, Rect))
        return;

    RoundFloatImageForBlock(OutImage, Destination.Format());
    const ::ULIS::eFormat srcFormat = ULISFormatForRawImageFormat(OutImage.Format);
    ::ULIS::FBlock src(OutImage.GetWidth(), OutImage.GetHeight(), srcFormat);
    CopyImageToBlock(OutImage, &src);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(Destination.Format());
    ctx.ConvertFormat(src, Destination, src.Rect(), ::ULIS::FVec2I(Rect.Min.X, Rect.Min.Y));
    ctx.Finish();
}
}

UOdysseyTextureLayerStack::~UOdysseyTextureLayerStack()
{
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
}

UOdysseyTextureLayerStack::UOdysseyTextureLayerStack()
{
    SupportedLayerClasses.Add(UOdysseyTextureLayerFolder::StaticClass());
    SupportedLayerClasses.Add(UOdysseyTextureLayerImageRaster::StaticClass());
    SupportedLayerClasses.Add(UOdysseyTextureLayerImageVector::StaticClass());

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyTextureLayerStack::OnRenderingChanged);
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddUObject(this, &UOdysseyTextureLayerStack::OnObjectPropertyChanged);
}

bool
UOdysseyTextureLayerStack::IsSRGB() const
{
    UTexture2D* texture = GetTexture();
    if (!texture)
        return false;

    const ETextureSourceFormat format = texture->Source.GetFormat();
    return (format == TSF_BGRA8 || format == TSF_G8) && texture->SRGB;
}

bool
UOdysseyTextureLayerStack::UsesLinearCompositing() const
{
    const UOdysseyTextureLayerStackUserData* userData = Cast<UOdysseyTextureLayerStackUserData>(GetOuter());
    return userData && userData->bLinearCompositing;
}

bool
UOdysseyTextureLayerStack::KeepsBytesOnSRGBChange() const
{
    const UOdysseyTextureLayerStackUserData* userData = Cast<UOdysseyTextureLayerStackUserData>(GetOuter());
    return !userData || userData->SRGBChangePolicy == EOdysseyTextureSRGBChangePolicy::KeepBytes;
}

EOdysseyColorConversion
UOdysseyTextureLayerStack::GetLayerColorConversion() const
{
    return IsSRGB() && !UsesLinearCompositing() ? EOdysseyColorConversion::Encode : EOdysseyColorConversion::None;
}

EOdysseyColorConversion
UOdysseyTextureLayerStack::GetDisplayColorConversion(const UOdysseyLayer* RenderedLayer) const
{
    return RenderedLayer && GetLayerColorConversion() == EOdysseyColorConversion::None && IsSRGB()
        ? EOdysseyColorConversion::Encode
        : EOdysseyColorConversion::None;
}

EOdysseyColorConversion
UOdysseyTextureLayerStack::GetContentColorConversion(const FOdysseyRenderContext* RenderContext) const
{
    return IsSRGB() && UsesLinearCompositing() ? EOdysseyColorConversion::Encode : EOdysseyColorConversion::None;
}

ETextureRenderTargetFormat
UOdysseyTextureLayerStack::GetCompositingFormat() const
{
    UTexture2D* texture = GetTexture();
    if (!texture)
        return RTF_RGBA8;

    switch (texture->Source.GetFormat())
    {
        // 16-bit and float textures are blended in 32-bit floats, which keep every 16-bit value.
        case TSF_R16F:
        case TSF_G16:
        case TSF_RGBA16:
        case TSF_RGBA16F:
        case TSF_R32F:
        case TSF_RGBA32F:
            return RTF_RGBA32f;

        default:
            // Linear colors need more than 8 bits to keep every sRGB encoded color.
            return IsSRGB() && UsesLinearCompositing() ? RTF_RGBA16f : RTF_RGBA8;
    }
}

void
UOdysseyTextureLayerStack::ColorSettingsChanged()
{
    for (UOdysseyLayer* layer : GetLayers())
    {
        if (UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layer))
            textureLayer->ColorSettingsChanged();
    }
    RenderingCompositionChanged(false);
}

void
UOdysseyTextureLayerStack::OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
{
    if (!Object || Object != GetTexture())
        return;

    // The sRGB setting changes how the layers interpret the bytes of the texture.
    // Changes without property name are ignored: texture compilations send them, while no texture may be compiled.
    if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UTexture, SRGB))
        ColorSettingsChanged();
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
    // The texture source, written below, holds the committed pixels only.
    RenderWithoutPreview();
    if (mRenderPreview)
    {
        mRenderPreview->OnInvalidated().Remove(mRenderPreviewInvalidatedHandle);
        mRenderPreview.Reset();
    }
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
    mPreviousMipSettings = texture->MipGenSettings;
    texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
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
    texture->MipGenSettings = mPreviousMipSettings;
    UpdateTextureSource();
}

void
UOdysseyTextureLayerStack::SetRenderPreview(TSharedPtr<FOdysseyRenderPreview> Preview)
{
    if (mRenderPreview == Preview)
    {
        return;
    }
    const bool bHadOverrides = HasPreviewOverrides();
    if (mRenderPreview)
    {
        mRenderPreview->OnInvalidated().Remove(mRenderPreviewInvalidatedHandle);
    }
    mRenderPreview = MoveTemp(Preview);
    if (mRenderPreview)
    {
        mRenderPreviewInvalidatedHandle = mRenderPreview->OnInvalidated().AddUObject(this, &UOdysseyTextureLayerStack::OnRenderPreviewInvalidated);
    }
    if (bHadOverrides || HasPreviewOverrides())
    {
        OnRenderPreviewInvalidated({});
    }
}

void
UOdysseyTextureLayerStack::RenderWithoutPreview()
{
    if (!HasPreviewOverrides() || !mTextureFastUpdateSurface.IsValid())
        return;

    if (!RenderTarget)
        CreateRenderTarget();

    UpdateRenderTargetFormat();
    FastUpdateTexture({ GetDefaultRenderRect() }, false);
}

bool
UOdysseyTextureLayerStack::HasPreviewOverrides() const
{
    if (!mRenderPreview)
        return false;

    // The preview may also replace nodes of other assets.
    const TMap<FGuid, FOdysseyRenderOverride>& overrides = mRenderPreview->GetContext().Overrides;
    for (const FGuid& id : GetRenderingComposition(EOdysseyRenderingType::Editor, 0))
    {
        if (overrides.Contains(id))
            return true;
    }
    return false;
}

void
UOdysseyTextureLayerStack::OnRenderPreviewInvalidated(const TArray<FIntRect>& Rects)
{
    if (Rects.IsEmpty())
    {
        mInvalidTileMap.Invalidate();
    }
    else
    {
        mInvalidTileMap.Invalidate(Rects);
    }

    if (mTextureUpdateMode == EOdysseyTextureLayerStackTextureUpdateMode::Instantaneous)
        UpdateTexture();
}

void
UOdysseyTextureLayerStack::FastUpdateTexture(const TArray<FIntRect>& iRects, bool bWithPreview)
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FTextureCompilingManager::Get().FinishCompilation({ texture });
    ETextureSourceFormat textureSourceFormat = texture->Source.GetFormat();

    const FOdysseyRenderContext* renderContext = bWithPreview && mRenderPreview ? &mRenderPreview->GetContext() : nullptr;
    for (const FIntRect& rect : iRects)
    {
        Render_GameThread(RenderTarget, EOdysseyRenderingType::Editor, rect, renderContext);
    }

    //Fence ?
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();

    // TODO(GPU-tools): the edited texture is read back to the CPU at each update; keep the texture source on the GPU while editing.
    TSharedPtr<::ULIS::FBlock> dst = mTextureFastUpdateSurface->Block();
    for (const FIntRect& rect : iRects)
    {
        ReadBackRect(RenderTarget, rect, *dst);
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

    // The saved source holds the committed pixels only.
    RenderWithoutPreview();
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
    if (HasPreviewOverrides())
    {
        OnRenderPreviewInvalidated({});
    }
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
            Render_GameThread(RenderTarget, EOdysseyRenderingType::Render, rect);
        }

        ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> dst = MakeShareable(NewBlockFromUTextureData(texture, format));

        //Fence ?
        for (const FIntRect& rect : invalidRects)
        {
            ReadBackRect(RenderTarget, rect, *dst);
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

    const ETextureRenderTargetFormat format = GetCompositingFormat();

    if (RenderTarget->RenderTargetFormat != format)
    {
        RenderTarget->RenderTargetFormat = format;
        RenderTarget->bForceLinearGamma = true;
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
UOdysseyTextureLayerStack::CreateRenderingRenderTarget() const
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return nullptr;

    FTextureCompilingManager::Get().FinishCompilation({ texture });

    const ETextureRenderTargetFormat format = GetCompositingFormat();

    UTextureRenderTarget2D* renderTarget = NewObject<UTextureRenderTarget2D>();
    renderTarget->RenderTargetFormat = format;
    renderTarget->bForceLinearGamma = true;
    renderTarget->SRGB = renderTarget->IsSRGB();
    renderTarget->InitAutoFormat(texture->GetSizeX(), texture->GetSizeY());

    return renderTarget;
}

UTexture2D*
UOdysseyTextureLayerStack::CreateExportTexture( const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory )
{
    FString packagePathName = iPackagePath / iAssetName;
    UPackage* package = CreatePackage( *packagePathName );
    if( !ensure(package) )
        return nullptr;

    FObjectDuplicationParameters params(GetTexture(), package );
    params.DestName = FName( iAssetName );
    params.ApplyFlags = RF_Public | RF_Standalone;

    UTexture2D* texture = Cast<UTexture2D>(StaticDuplicateObjectEx(params));
    texture->RemoveUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass());

    return texture;
}
