// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerRoot.h"
#include "OdysseyTextureLayerFolder.h"
#include "OdysseyRasterBlock.h"
#include "TextureCompiler.h"
#include "UObject/ObjectSaveContext.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyRectUtils.h"
#include "OdysseyRasterBlockMutator.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "OdysseyTextureLayerImageVector.h"
#include "ULISLoaderModule.h"
#include "OdysseyPixelFormat.h"
#include "UObject/Package.h"
#include "OdysseyImageRenderer.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
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

    //Set the layer as Current Layer
    layerStack->AddLayersToHierarchy({ layer }, layerStack->LayerRoot, 0);
    layerStack->CurrentLayer = layer;

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
    layerStack->AddLayersToHierarchy({ layer }, layerStack->LayerRoot, 0);
    layerStack->CurrentLayer = layer;

    return layerStack;
}

UOdysseyTextureLayerStack::~UOdysseyTextureLayerStack()
{
    FOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
}

UOdysseyTextureLayerStack::UOdysseyTextureLayerStack()
{
    CompatibleLayers.Add(UOdysseyTextureLayerFolder::StaticClass());
    CompatibleLayers.Add(UOdysseyTextureLayerImageRaster::StaticClass());
    CompatibleLayers.Add(UOdysseyTextureLayerImageVector::StaticClass());

    LayerRootClass = UOdysseyTextureLayerRoot::StaticClass();
    SRGB = false;

    FOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyTextureLayerStack::OnRenderingChanged);
    RenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>("RenderTarget");
    RenderTarget->RenderTargetFormat = RTF_RGBA8;
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

int
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
}

::ULIS::eFormat
UOdysseyTextureLayerStack::GetFormat() const
{
    UTexture2D* texture = GetTexture();
    if (!texture)
        return Super::GetFormat();

    return ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
}

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

TArray<FIntRect>
UOdysseyTextureLayerStack::GetRenderingRects() const
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return {};

    return { FIntRect(0, 0, texture->Source.GetSizeX(), texture->Source.GetSizeY()) };
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

    //mRenderer = BuildImageRenderer(EOdysseyRenderingType::Render);

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

    if (RenderTarget->SizeX != GetWidth() || RenderTarget->SizeY != GetHeight())
        RenderTarget->ResizeTarget(GetWidth(), GetHeight());

    RenderTarget->UpdateResourceImmediate();

    RenderToTexture(RenderTarget, FFrameNumber(0), iRects);

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

    if ( mTextureFastUpdateSurface.IsValid() )
    {
        FastUpdateTexture(mInvalidTileMap.InvalidRects());
    }
    else
    {
        FTextureCompilingManager::Get().FinishCompilation({ texture });

        if (RenderTarget->SizeX != GetWidth() || RenderTarget->SizeY != GetHeight())
            RenderTarget->ResizeTarget(GetWidth(), GetHeight());

        RenderTarget->UpdateResourceImmediate();

        TArray<FIntRect> invalidRects = mInvalidTileMap.InvalidRects();
        RenderToTexture(RenderTarget, FFrameNumber(0), invalidRects);

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
