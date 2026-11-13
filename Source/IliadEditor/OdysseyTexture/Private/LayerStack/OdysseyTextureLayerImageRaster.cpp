// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerImageRaster.h"

#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "HAL/IConsoleManager.h"
#include "RenderGraphUtils.h"
#include "UObject/UObjectIterator.h"
#include "ScreenPass.h"
#include "TextureCompiler.h"
#include "TextureResource.h"
#include "UObject/ObjectSaveContext.h"

#include "OdysseyBlendShader.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyStyle.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyRasterBlockUndo.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTextureLayerImageRasterImport.h"
#include "OdysseyTextureLayerImageRasterExport.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTiledImage.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"

#define LOCTEXT_NAMESPACE "Texture"

namespace
{
TAutoConsoleVariable<int32> CVarUseTiledRendering(
    TEXT("Odyssey.Texture.UseTiledRendering"), 0,
    TEXT("Experimental raster texture layer rendering: 0 = texture, 1 = tiled image."),
    FConsoleVariableDelegate::CreateLambda([](IConsoleVariable*)
    {
        // Refresh previews and thumbnails when switching the comparison path.
        for (TObjectIterator<UOdysseyTextureLayerImageRaster> It; It; ++It)
        {
            if (!It->HasAnyFlags(RF_ClassDefaultObject) && It->GetTexture())
            {
                It->RenderingChanged();
            }
        }
    }));
}

UOdysseyTextureLayerImageRaster::~UOdysseyTextureLayerImageRaster()
{
}

UOdysseyTextureLayerImageRaster::UOdysseyTextureLayerImageRaster()
{
    LayerTypeName = LOCTEXT("layer-image-raster.type", "Raster Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerBitmap16");
}

void
UOdysseyTextureLayerImageRaster::PostLoad()
{
    Super::PostLoad();
    bTiledImageDirty = true;

    if (RasterBlock)
    {
        ConvertRasterBlock();
        SanitizeRasterBlock({RasterBlock->GetRect()}); //ensure the RasterBlock contains the right data
        UnbindRasterBlockDelegates();
        BindRasterBlockDelegates();

        //Copy the rasterblock data into the texture
        UTexture2D* renderTexture = GetRenderTexture();
        InitTextureWithBlockData(RasterBlock->GetBlock().Get(), renderTexture, renderTexture->Source.GetFormat());
        renderTexture->UpdateResource();
        FTextureCompilingManager::Get().FinishCompilation({ renderTexture });
    }
}

const UOdysseyTiledImage* UOdysseyTextureLayerImageRaster::GetTiledImage() const
{
    return TiledImage;
}

bool UOdysseyTextureLayerImageRaster::UpdateTiledImage()
{
    check(IsInGameThread());
    // Detached layers and class defaults have no source image.
    if (HasAnyFlags(RF_ClassDefaultObject) || !GetTexture())
    {
        return false;
    }

    TRACE_CPUPROFILER_EVENT_SCOPE(OdysseyTextureLayer_SyncTiledImage);
    UTexture2D* SourceTexture = GetRenderTexture();
    if (!SourceTexture || !SourceTexture->Source.IsValid())
    {
        return false;
    }
    const FIntPoint SourceSize(SourceTexture->Source.GetSizeX(), SourceTexture->Source.GetSizeY());
    const FGuid SourceId = SourceTexture->Source.GetId();
    // Source-only edits have no raster notifications. Preserve partial updates for normal commits.
    bTiledImageDirty |= !TiledImage || TiledImageSize != SourceSize
        || (PendingTiledImageTiles.IsEmpty() && TiledImageSourceId != SourceId);
    if (!bTiledImageDirty && PendingTiledImageTiles.IsEmpty())
    {
        return true;
    }
    SourceTexture->BlockOnAnyAsyncBuild();
    SourceTexture->WaitForPendingInitOrStreaming();

    // Integer 16-bit sources require 32-bit float tiles to preserve their precision.
    const ETextureSourceFormat SourceFormat = SourceTexture->Source.GetFormat();
    const EOdysseyTiledImageFormat TileFormat = SourceFormat == TSF_BGRA8
        ? EOdysseyTiledImageFormat::RGBA8
        : SourceFormat == TSF_RGBA16F ? EOdysseyTiledImageFormat::RGBA16F : EOdysseyTiledImageFormat::RGBA32F;

    if (!TiledImage)
    {
        TiledImage = NewObject<UOdysseyTiledImage>(this);
    }
    const FIntRect SourceRect(FIntPoint::ZeroValue, SourceSize);
    if (bTiledImageDirty || TiledImage->GetFormat() != TileFormat)
    {
        // Structural changes discard old tiles, including those outside a resized canvas.
        TiledImage->Initialize(64, TileFormat);
        TiledImage->ClearFlags(RF_Transactional);
        TiledImage->CopyFromTexture(SourceTexture, SourceRect, FIntPoint::ZeroValue);
    }
    else
    {
        // Upload each touched tile once, preserving identities and cache entries of all other tiles.
        for (const FIntPoint& Tile : PendingTiledImageTiles)
        {
            FIntRect Rect(Tile * 64, (Tile + FIntPoint(1, 1)) * 64);
            Rect.Clip(SourceRect);
            if (!Rect.IsEmpty())
            {
                TiledImage->CopyFromTexture(SourceTexture, Rect, Rect.Min);
            }
        }
    }
    PendingTiledImageTiles.Reset();
    TiledImageSize = SourceSize;
    TiledImageSourceId = SourceId;
    bTiledImageDirty = false;
    return true;
}

void UOdysseyTextureLayerImageRaster::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);
    if (!SaveContext.IsCooking())
    {
        UpdateTiledImage();
    }
}

void UOdysseyTextureLayerImageRaster::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
    Super::PostDuplicate(DuplicateMode);
    bTiledImageDirty = true;
}

void UOdysseyTextureLayerImageRaster::PostTransacted(const FTransactionObjectEvent& TransactionEvent)
{
    Super::PostTransacted(TransactionEvent);
    bTiledImageDirty = true;
}

void
UOdysseyTextureLayerImageRaster::ConvertRasterBlock()
{
    UTexture2D* renderTexture = GetRenderTexture();
    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(renderTexture->Source.GetFormat());
    RasterBlock->ConvertTo(RasterBlock->GetWidth(), RasterBlock->GetHeight(), format);
}

void
UOdysseyTextureLayerImageRaster::InitRasterBlock()
{
    UTexture2D* texture = GetTexture();
    if (!texture)
        return;

    UnbindRasterBlockDelegates();

    int width = texture->Source.GetSizeX();
    int height = texture->Source.GetSizeY();

    UTexture2D* renderTexture = GetRenderTexture();
    if ( renderTexture && renderTexture->Source.GetFormat() != TSF_Invalid )
    {
        ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(renderTexture->Source.GetFormat());
        RasterBlock = MakeShared<FOdysseyRasterBlock>(const_cast<UOdysseyTextureLayerImageRaster*>(this), width, height, format);

        TSharedPtr<::ULIS::FBlock> textureBlock = MakeShareable(NewBlockFromUTextureData(renderTexture, format));
        FOdysseyRasterBlockMutator rasterBlockMutator(RasterBlock, false);
        rasterBlockMutator.Copy(textureBlock, { textureBlock->Rect() });
        rasterBlockMutator.Commit();
    }
    else
    {
        //The layer a different texture with different parameters
        //Ensure the block uses those parameters
        ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
        //let's ensure the format has alpha, so add alpha channel of needed
        format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );

        RasterBlock = MakeShared<FOdysseyRasterBlock>(const_cast<UOdysseyTextureLayerImageRaster*>(this), width, height, format);
    }

    BindRasterBlockDelegates();
}

void
UOdysseyTextureLayerImageRaster::BindRasterBlockDelegates()
{
    RasterBlock->OnBlockChanged().AddUObject(const_cast<UOdysseyTextureLayerImageRaster*>(this), &UOdysseyTextureLayerImageRaster::OnBlockChanged);
    RasterBlock->OnBlockCommited().AddUObject(const_cast<UOdysseyTextureLayerImageRaster*>(this), &UOdysseyTextureLayerImageRaster::OnBlockCommited);
    RasterBlock->PostProcess().BindUObject(const_cast<UOdysseyTextureLayerImageRaster*>(this), &UOdysseyTextureLayerImageRaster::RasterBlockPostProcess);
}

void
UOdysseyTextureLayerImageRaster::UnbindRasterBlockDelegates()
{
    if (!RasterBlock)
        return;

    RasterBlock->OnBlockChanged().RemoveAll(this);
    RasterBlock->OnBlockCommited().RemoveAll(this);
}

void
UOdysseyTextureLayerImageRaster::InitTexture()
{
    Super::InitTexture();
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyTextureLayerImageRaster::GetRasterBlock() const
{
    if (!RasterBlock)
        const_cast<UOdysseyTextureLayerImageRaster*>(this)->InitRasterBlock();

    return RasterBlock;
}

void
UOdysseyTextureLayerImageRaster::SanitizeRasterBlock(const TArray<::ULIS::FRectI>& iRects)
{
    UTexture2D* texture = GetTexture();
    if (!texture)
        return;
    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    //let's ensure the format has alpha, so add alpha channel of needed
    format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );

    TSharedPtr<::ULIS::FBlock> block = GetRasterBlock()->GetBlock();
    if (block->Format() != format)
    {
        for (const ::ULIS::FRectI& rect : iRects)
        {
            TSharedPtr<::ULIS::FBlock> convBlock = MakeShared<::ULIS::FBlock>(rect.w, rect.h, format);

            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
            ctx.ConvertFormat(
                *block.Get(),
                *convBlock.Get(),
                rect,
                ::ULIS::FVec2I(0),
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient
            );
            ctx.Finish();

            ctx.ConvertFormat(
                *convBlock.Get(),
                *block.Get(),
                ::ULIS::FRectI::Auto,
                rect.Position(),
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient
            );
            ctx.Finish();
        }
    }
}

void UOdysseyTextureLayerImageRaster::InvalidateTiledImage(const TArray<::ULIS::FRectI>& Rects)
{
    if (bTiledImageDirty)
    {
        return;
    }
    const FIntRect Bounds = GetDefaultRenderRect();
    for (FIntRect Rect : ULISUtils::ToIntRects(Rects))
    {
        Rect.Clip(Bounds);
        if (Rect.IsEmpty())
        {
            continue;
        }
        // FIntRect::Max is exclusive. A rectangle ending at 64 must not touch tile 1.
        const FIntPoint First = Rect.Min / 64;
        const FIntPoint Last = (Rect.Max - FIntPoint(1, 1)) / 64;
        for (int32 Y = First.Y; Y <= Last.Y; ++Y)
        {
            for (int32 X = First.X; X <= Last.X; ++X)
            {
                PendingTiledImageTiles.Add(FIntPoint(X, Y));
            }
        }
    }
}

bool UOdysseyTextureLayerImageRaster::BuildRenderPipelineInternal(FFrameNumber Frame, uint64 Type,
    FRenderFunction& OutRenderFunction, const FCanRenderFunction& CanRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& Parents) const
{
    if (!CVarUseTiledRendering.GetValueOnGameThread())
    {
        return Super::BuildRenderPipelineInternal(Frame, Type, OutRenderFunction, CanRenderFunction, Parents);
    }
    if (!const_cast<UOdysseyTextureLayerImageRaster*>(this)->UpdateTiledImage())
    {
        return Super::BuildRenderPipelineInternal(Frame, Type, OutRenderFunction, CanRenderFunction, Parents);
    }
    const UOdysseyTiledImage* Image = GetTiledImage();
    const FIntPoint Size = TiledImageSize;
    const EPixelFormat Format = Image->GetPixelFormat();
    auto DrawTiles = Image->BuildRenderFunction(FIntRect(FIntPoint::ZeroValue, Size));
    OutRenderFunction = [DrawTiles = MoveTemp(DrawTiles), Size, Format](FRDGBuilder& GraphBuilder,
        ERHIFeatureLevel::Type FeatureLevel, FRDGTextureRef Destination,
        const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& Transform)
    {
        RDG_EVENT_SCOPE(GraphBuilder, "Odyssey.Texture.TiledLayer");
        // Assemble a continuous source before filtering: atlas borders must not create seams
        // under zoom, rotation or anisotropic sampling. No persistent full-size texture is added.
        FRDGTextureRef Source = GraphBuilder.CreateTexture(FRDGTextureDesc::Create2D(Size, Format,
            FClearValueBinding::Transparent, TexCreate_ShaderResource | TexCreate_RenderTargetable),
            TEXT("Odyssey.Texture.TiledLayer.Source"));
        DrawTiles(GraphBuilder, Source, FIntPoint::ZeroValue);
        AddClearRenderTargetPass(GraphBuilder, Destination, FLinearColor::Transparent, DestinationRect);
        FOdysseyBlendShader::BlendRect(GraphBuilder, FeatureLevel, Destination, Source, Destination,
            SourceRect, DestinationRect, Transform, EOdysseyColorBlendMode::Normal,
            EOdysseyAlphaBlendMode::Normal, 1.0f, EOdysseyAntiAliasing::Anisotropic);
    };
    return true;
}

void
UOdysseyTextureLayerImageRaster::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    InvalidateTiledImage(iRects);
    SanitizeRasterBlock(iRects);
    FOdysseySurfaceTexture2DEditable surface(GetRenderTexture(), GetRasterBlock()->GetBlock());
    surface.Invalidate(iRects);
    RenderingChanged(::ULISUtils::ToIntRects(iRects), true);
}

void
UOdysseyTextureLayerImageRaster::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
    InvalidateTiledImage(iRects);
    TSharedPtr<::ULIS::FBlock> block =  GetRasterBlock()->GetBlock();
    FOdysseySurfaceTexture2DEditable surface(GetRenderTexture(), block);
    surface.Invalidate(iRects);

    TArray<FIntRect> intRects = ::ULISUtils::ToIntRects(iRects);
    CopyBlockDataToTextureSource(block.Get(), GetRenderTexture(), intRects);
    RenderingChanged(::ULISUtils::ToIntRects(iRects), false);
}

void
UOdysseyTextureLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    if( !IsEditable() )
        return;

    Modify();

    FIntRect rect = GetDefaultRenderRect();
    TStrongObjectPtr<UTextureRenderTarget2D> layerRenderTarget(CreateRenderingRenderTarget());
    TStrongObjectPtr<UTextureRenderTarget2D> destinationRenderTarget(CreateRenderingRenderTarget());

    //Clear the destination rendertarget before blending on it
    ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
        [destinationRenderTarget, rect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef destinationTexture = destinationRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
            AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent, rect);
            graphBuilder.Execute();
        }
    );

    for ( UOdysseyLayer* layer : iLayers )
    {
        layer->Render_GameThread(
            layerRenderTarget.Get(),
            FFrameNumber(0),
            EOdysseyRenderingType::Render
        );

        const ERHIFeatureLevel::Type featureLevel = GMaxRHIFeatureLevel;

        ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
            [layerRenderTarget, destinationRenderTarget, featureLevel, rect, layer](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);

                FRDGTextureRef layerTexture = layerRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                FRDGTextureRef destinationTexture = destinationRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

                EOdysseyColorBlendMode ColorBlendMode = GetColorBlendModeFromBlendMode(layer->GetBlendMode());
                EOdysseyAlphaBlendMode AlphaBlendMode = GetAlphaBlendModeFromBlendMode(layer->GetBlendMode());

                FOdysseyBlendShader::BlendRect(
                    graphBuilder,
                    featureLevel,
                    destinationTexture,
                    layerTexture,
                    destinationTexture,
                    rect,
                    rect,
                    FMatrix::Identity,
                    ColorBlendMode,
                    layer->GetInheritsAlpha() ? EOdysseyAlphaBlendMode::Back : AlphaBlendMode,
                    layer->GetOpacity(),
                    EOdysseyAntiAliasing::NearestNeighbor
                );

                graphBuilder.Execute();
            }
        );
    }

    FImage OutImage;
    if (!FImageUtils::GetRenderTargetImage(destinationRenderTarget.Get(), OutImage))
        return;

    ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock( rect.Width(), rect.Height(), format ));
    CopyImageToBlock(OutImage, block.Get());

    FOdysseyRasterBlockMutator mutator( GetRasterBlock() );
    mutator.Copy(block, {::ULISUtils::ToULISRectI(rect)});
    mutator.Commit();
}

FOdysseyMediaProvider
UOdysseyTextureLayerImageRaster::GetMediaProvider(uint32 iFrameIndex) const
{
    bool isActive = IsActivatedRecursively();
    bool isLocked = IsLockedRecursively();

    FOdysseyMediaProvider mediaProvider;
    mediaProvider.IsLocked(!isActive || isLocked);

    TSharedPtr<FOdysseyMediaRaster> mediaRaster = MakeShared<FOdysseyMediaRaster>(GetRasterBlock());
    mediaProvider.Add(mediaRaster);
    return mediaProvider;
}

/* struct FOdysseyTextureLayerImageRasterObjectVersion
{
    enum Type
    {
        // Before any version changes were made
        BeforeCustomVersionWasAdded,

        // Stopped relying on Raster Block and use UTexture2D instead
        NoRasterBlockSaving,

        // -----<new versions can be added above this line>-------------------------------------------------
        VersionPlusOne,
        LatestVersion = VersionPlusOne - 1
    };

    // The GUID for this custom version number
    const static FGuid GUID;

private:
    FOdysseyTextureLayerImageRasterObjectVersion() {}
};

const FGuid FOdysseyTextureLayerImageRasterObjectVersion::GUID(0xD5CF1CCA, 0xEAC04893, 0x9F032A90, 0x2D3E968F);
FDevVersionRegistration GRegisterOdysseyTextureLayerImageRasterObjectVersion(FOdysseyTextureLayerImageRasterObjectVersion::GUID, FOdysseyTextureLayerImageRasterObjectVersion::LatestVersion, TEXT("OdysseyTextureLayerImageRaster")); */

void
UOdysseyTextureLayerImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    if( Ar.IsSaving() )
    {
        //FOdysseyTextureLayerImageRasterExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyTextureLayerImageRasterImport::Read( this, Ar ))
        {
            //PATCH: allows us to load most of the old textures without implying new textures crash
            //This is needed because we do not write RasterBlock in save anymore
            //And when FOdysseyTextureLayerImageRasterImport::Read() returns false, it can mean 2 things :
            // - There is nothing to read (happens with recent textures)
            // - There is something to read in an old fashioned way (happens with really old textures)
            //This code is not perfect, but should do the trick, and we will be able to remove it in the future
            //when we consider that really old projects should not be openable anymore (Example: the Roller_board_long storyboard project made by Antoine Antin)
            uint64 start = Ar.Tell();
            uint64 end = Ar.TotalSize();
            uint64 neededSize = sizeof(uint32) * 7; //Id(4*uint32) + width(1*uint32) + height(1*uint32) + format(1*uint32)
            if (end - start >= neededSize) //check if we can read chunkID and chunkLen
            {
                FGuid id;
                int width;
                int height;

                Ar << id; //uint32 * 4
                Ar << width; //int32
                Ar << height; //int32

                Ar.Seek(start);

                bool canRead = id.IsValid() && width > 0 && width <= 8192 && height > 0 && height <= 8192;
                if (canRead)
                {
                    RasterBlock = MakeShared<FOdysseyRasterBlock>(this);
                    Ar << *RasterBlock;
                }
            }
        }
    }
}

TArray<::ULIS::FEvent>
UOdysseyTextureLayerImageRaster::RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FOdysseyInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsAlphaLocked())
        return iWaitList;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlock();

    //Apply AlphaLock
    TArray<::ULIS::FEvent> events;
    TSharedPtr<::ULIS::FBlock> block = rasterBlock->GetBlock();
    TArray<FIntPoint> invalidTiles = iInvalidMap.InvalidTiles();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
    for (const FIntPoint& invalidTile : invalidTiles)
    {
        TSharedPtr<::ULIS::FBlock> originalBlock = iOriginalBlocks[invalidTile];
        ::ULIS::FRectI rect = ::ULISUtils::ToULISRectI(iInvalidMap.GetTileRect(invalidTile));
        ::ULIS::FEvent eventBlend;
        ctx.Blend(
            *originalBlock
            , *block
            , ::ULIS::FRectI::Auto
            , rect.Position()
            , ::ULIS::Blend_Back
            , ::ULIS::Alpha_Top
            , 1.f
            , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
            , iWaitList.Num()
            , iWaitList.GetData()
            , &eventBlend
        );
        events.Add(eventBlend);
    }

    return events;
}

TArray<FGuid>
UOdysseyTextureLayerImageRaster::GetRenderingComposition(uint64 iRenderType, int iFrame) const
{
    return { GetRenderingId() };
}

void
UOdysseyTextureLayerImageRaster::SetIsAlphaLocked(bool Value)
{
    if( !IsEditable() )
        return;

    Modify();

    bIsAlphaLocked = Value;
}

bool
UOdysseyTextureLayerImageRaster::IsAlphaLocked() const
{
    return bIsAlphaLocked;
}

/* #if WITH_EDITOR

void
UOdysseyTextureLayerImageRaster::ImportTexture(UTexture2D* InTexture)
{
    UTexture2D* renderTexture = GetRenderTexture();
    if ( !renderTexture )
        return;

    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    bool expectSRGB = (texture->Source.GetFormat() == TSF_BGRA8 || texture->Source.GetFormat() == TSF_G8) && texture->SRGB;

    FImage srcImage;
    InTexture->Source.GetMipImage(srcImage, 0);

    if (expectSRGB && srcImage.GetGammaSpace() == EGammaSpace::sRGB)
    {
        //We don't need a conversion here
        //so just set srcImage.GammaSpace to Linear to avoid gamma conversion
        srcImage.GammaSpace = EGammaSpace::Linear;
    }

    FTextureSource::FMipLock dstLock(FTextureSource::ELockState::ReadWrite, &renderTexture->Source, 0);

    if (srcImage.SizeX == dstLock.Image.SizeX && srcImage.SizeY == dstLock.Image.SizeY)
    {
        FImageCore::CopyImage(srcImage, dstLock.Image);
    }
    else
    {
        //Copy no crop
        int64 srcBpp = ERawImageFormat::GetBytesPerPixel(srcImage.Format);
        int64 dstBpp = ERawImageFormat::GetBytesPerPixel(dstLock.Image.Format);

        int32 sizeX = FMath::Min(dstLock.Image.SizeX, srcImage.SizeX);
        int32 sizeY = FMath::Min(dstLock.Image.SizeY, srcImage.SizeY);

        FImage srcFullImage(sizeX, sizeY, srcImage.Format, srcImage.GetGammaSpace());
        int64 srcNumBytes = srcFullImage.GetImageSizeBytes();
        srcFullImage.RawData.Empty(srcNumBytes);
        srcFullImage.RawData.AddZeroed(srcNumBytes);

        FImage dstFullImage(sizeX, sizeY, dstLock.Image.Format, dstLock.Image.GetGammaSpace());
        int64 dstNumBytes = dstFullImage.GetImageSizeBytes();
        dstFullImage.RawData.Empty(dstNumBytes);
        dstFullImage.RawData.AddZeroed(dstNumBytes);

        for (int y = 0; y < sizeY; y++)
        {
            uint8* srcLine = (uint8*)srcImage.GetPixelPointer(0, y);
            uint8* dstLine = (uint8*)srcFullImage.GetPixelPointer(0, y);
            FMemory::Memcpy(dstLine, srcLine, sizeX * srcBpp);
        }

        for (int y = 0; y < sizeY; y++)
        {
            uint8* srcLine = (uint8*)dstLock.Image.GetPixelPointer(0, y);
            uint8* dstLine = (uint8*)dstFullImage.GetPixelPointer(0, y);
            FMemory::Memcpy(dstLine, srcLine, sizeX * dstBpp);
        }

        //CopyImage makes the conversion between formats and Gammaspace
        FImageCore::CopyImage(srcFullImage, dstFullImage);

        for (int y = 0; y < sizeY; y++)
        {
            uint8* srcLine = (uint8*)dstFullImage.GetPixelPointer(0, y);
            uint8* dstLine = (uint8*)dstLock.Image.GetPixelPointer(0, y);
            FMemory::Memcpy(dstLine, srcLine, sizeX * dstBpp);
        }
    }

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(renderTexture->Source.GetFormat());
    TSharedPtr<::ULIS::FBlock> textureBlock = MakeShareable(NewBlockFromUTextureData(renderTexture, format));

    FOdysseyRasterBlockMutator rasterBlockMutator(GetRasterBlock());
    rasterBlockMutator.Copy(textureBlock, { textureBlock->Rect() });
    rasterBlockMutator.Commit();
}

#endif */

#undef LOCTEXT_NAMESPACE
