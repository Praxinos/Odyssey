// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCellImageRaster.h"

#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "OdysseyAnimation.h"

#if WITH_EDITOR
#include "Engine/Texture2D.h"
#include "HAL/IConsoleManager.h"
#include "Misc/App.h"
#include "OdysseyBlendShader.h"
#include "OdysseyTiledImage.h"
#include "UObject/UObjectIterator.h"
#include "ULISLoaderModule.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyAnimationCellImageRasterExport.h"
#include "OdysseyAnimationCellImageRasterImport.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyRasterBlock.h"
#include "ULISUtils.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "OdysseyPixelFormat.h"
#include "TextureCompiler.h"
#include "TextureResource.h"
#include "UObject/ObjectSaveContext.h"
#include "OdysseyRasterBlockMutator.h"
#include "ScopedTransaction.h"

#endif

#define LOCTEXT_NAMESPACE "Animation"

#if WITH_EDITOR
namespace
{
TAutoConsoleVariable<int32> CVarUseTiledAnimationRendering(
    TEXT("Odyssey.Animation.UseTiledRendering"), 0,
    TEXT("Experimental raster animation cell rendering in the editor: 0 = texture, 1 = tiled image."),
    FConsoleVariableDelegate::CreateLambda([](IConsoleVariable*)
    {
        for (TObjectIterator<UOdysseyAnimationCellImageRaster> It; It; ++It)
        {
            if (!It->HasAnyFlags(RF_ClassDefaultObject) && It->GetAnimation())
            {
                It->RenderingChanged();
            }
        }
    }));
}
#endif

UOdysseyAnimationCellImageRaster::UOdysseyAnimationCellImageRaster()
{
}

TArray<FGuid>
UOdysseyAnimationCellImageRaster::GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const
{
    return { GetRenderingId() };
}

FIntRect
UOdysseyAnimationCellImageRaster::GetDefaultRenderRect() const
{
    return FIntRect(0, 0, GetAnimation()->GetWidth(), GetAnimation()->GetHeight());
}

bool
UOdysseyAnimationCellImageRaster::BuildRenderPipelineInternal(
    FFrameNumber iFrame,
    uint64 iType,
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents
) const
{
#if WITH_EDITOR
    if (CVarUseTiledAnimationRendering.GetValueOnGameThread()
        && const_cast<UOdysseyAnimationCellImageRaster*>(this)->UpdateTiledImage())
    {
        const UOdysseyTiledImage* Image = GetTiledImage();
        const FMatrix CellTransform = (iType & EOdysseyRenderingType::OutOfPegs) ? OutOfPegsTransform() : FMatrix::Identity;
        const FIntPoint Size = TiledImageSize;
        // Reassemble using the legacy texture's sampling representation. In particular an
        // sRGB render target encodes the stored linear tiles before hardware interpolation.
        const UTexture2D* SourceTexture = GetRenderTexture();
        const EPixelFormat Format = SourceTexture->GetPixelFormat();
        const ETextureCreateFlags EncodingFlags = SourceTexture->GetResource()->TextureRHI->GetDesc().Flags & TexCreate_SRGB;
        auto DrawTiles = Image->BuildRenderFunction(FIntRect(FIntPoint::ZeroValue, Size));
        oRenderFunction = [DrawTiles = MoveTemp(DrawTiles), Size, Format, EncodingFlags, CellTransform](FRDGBuilder& GraphBuilder,
            ERHIFeatureLevel::Type FeatureLevel, FRDGTextureRef Destination,
            const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& Transform)
        {
            RDG_EVENT_SCOPE(GraphBuilder, "Odyssey.Animation.TiledCell");
            // Assemble a continuous source before filtering: atlas borders must not create seams
            // under zoom, rotation or anisotropic sampling. No persistent full-size texture is added.
            FRDGTextureRef Source = GraphBuilder.CreateTexture(FRDGTextureDesc::Create2D(Size, Format,
                FClearValueBinding::Transparent, TexCreate_ShaderResource | TexCreate_RenderTargetable | EncodingFlags),
                TEXT("Odyssey.Animation.TiledCell.Source"));
            DrawTiles(GraphBuilder, Source, FIntPoint::ZeroValue);
            AddClearRenderTargetPass(GraphBuilder, Destination, FLinearColor::Transparent, DestinationRect);
            FOdysseyBlendShader::BlendRect(GraphBuilder, FeatureLevel, Destination, Source, Destination,
                SourceRect, DestinationRect, CellTransform * Transform, EOdysseyColorBlendMode::Normal,
                EOdysseyAlphaBlendMode::Normal, 1.0f, EOdysseyAntiAliasing::Anisotropic);
        };
        return true;
    }
#endif
    return Super::BuildRenderPipelineInternal(iFrame, iType, oRenderFunction, iCanRenderFunction, iParents);
}

#if WITH_EDITOR
const UOdysseyTiledImage* UOdysseyAnimationCellImageRaster::GetTiledImage() const
{
    return Cast<UOdysseyTiledImage>(TiledImage);
}

bool UOdysseyAnimationCellImageRaster::UpdateTiledImage()
{
    check(IsInGameThread());
    // Detached layers and class defaults have no source image.
    if (HasAnyFlags(RF_ClassDefaultObject) || !GetAnimation() || !FApp::CanEverRender())
    {
        return false;
    }

    TRACE_CPUPROFILER_EVENT_SCOPE(OdysseyAnimationCell_SyncTiledImage);
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

    // CopyFromTexture samples linear values. Decoded sRGB values need float storage, and
    // float sources keep their GPU precision so filtering matches the existing render path.
    const EPixelFormat SourcePixelFormat = SourceTexture->GetPixelFormat();
    const EOdysseyTiledImageFormat TileFormat = SourcePixelFormat == PF_FloatRGBA
        ? EOdysseyTiledImageFormat::RGBA16F
        : SourceTexture->Source.GetFormat() == TSF_BGRA8 && !SourceTexture->SRGB
            ? EOdysseyTiledImageFormat::RGBA8 : EOdysseyTiledImageFormat::RGBA32F;

    UOdysseyTiledImage* Image = Cast<UOdysseyTiledImage>(TiledImage);
    if (!Image)
    {
        Image = NewObject<UOdysseyTiledImage>(this);
        TiledImage = Image;
    }
    const FIntRect SourceRect(FIntPoint::ZeroValue, SourceSize);
    if (bTiledImageDirty || Image->GetFormat() != TileFormat)
    {
        // Structural changes discard old tiles, including those outside a resized canvas.
        Image->Initialize(64, TileFormat);
        Image->ClearFlags(RF_Transactional);
        Image->CopyFromTexture(SourceTexture, SourceRect, FIntPoint::ZeroValue);
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
                Image->CopyFromTexture(SourceTexture, Rect, Rect.Min);
            }
        }
    }
    PendingTiledImageTiles.Reset();
    TiledImageSize = SourceSize;
    TiledImageSourceId = SourceId;
    bTiledImageDirty = false;
    return true;
}

void UOdysseyAnimationCellImageRaster::InvalidateTiledImage(const TArray<::ULIS::FRectI>& Rects)
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

void UOdysseyAnimationCellImageRaster::PostTransacted(const FTransactionObjectEvent& TransactionEvent)
{
    Super::PostTransacted(TransactionEvent);
    bTiledImageDirty = true;
}

void
UOdysseyAnimationCellImageRaster::InitRasterBlock() const
{
    if ( !mRasterBlock )
    {
        int width = GetAnimation()->GetWidth();
        int height = GetAnimation()->GetHeight();

        UTexture2D* texture = GetRenderTexture();
        if ( texture && texture->Source.GetFormat() != TSF_Invalid )
        {
            ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
            mRasterBlock = MakeShared<FOdysseyRasterBlock>(const_cast<UOdysseyAnimationCellImageRaster*>(this), width, height, format);

            TSharedPtr<::ULIS::FBlock> textureBlock = MakeShareable(NewBlockFromUTextureData(texture, format));
            FOdysseyRasterBlockMutator rasterBlockMutator(mRasterBlock, false);
            rasterBlockMutator.Copy(textureBlock, { textureBlock->Rect() });
            rasterBlockMutator.Commit();
        }
        else
        {
            ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
            switch ( GetAnimation()->GetFormat() )
            {
                case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8; break;
                case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF; break;
            }

            mRasterBlock = MakeShared<FOdysseyRasterBlock>(const_cast<UOdysseyAnimationCellImageRaster*>(this), width, height, format);
        }
    }

    mRasterBlock->OnBlockChanged().RemoveAll(this);
    mRasterBlock->OnBlockCommited().RemoveAll(this);

    mRasterBlock->OnBlockChanged().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockChanged);
    mRasterBlock->OnBlockCommited().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockCommited);
    mRasterBlock->PostProcess().BindUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::RasterBlockPostProcess);
}

void
UOdysseyAnimationCellImageRaster::InitTexture()
{
    Super::InitTexture();

    UTexture2D* texture = GetRenderTexture();
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlock(); //ensures mRasterBlock exists
    InitTextureWithBlockData(rasterBlock->GetBlock().Get(), texture , TextureSourceFormatForULISFormat(rasterBlock->GetFormat()));
    texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ texture  });
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyAnimationCellImageRaster::GetRasterBlock() const
{
    if (!mRasterBlock)
        InitRasterBlock();

    return mRasterBlock;
}

void
UOdysseyAnimationCellImageRaster::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);
    bTiledImageDirty = true;

    //The cell could be duplicated in a different animation with different parameters
    //Ensure the block uses those parameters
    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(GetAnimation()->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8; break;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF; break;
    }

    int width = GetAnimation()->GetWidth();
    int height = GetAnimation()->GetHeight();

    if (mRasterBlock)
    {
        mRasterBlock->PostDuplicate();
        mRasterBlock->ConvertTo(width, height, format);

        InitRasterBlock();
    }
}

void
UOdysseyAnimationCellImageRaster::PostLoad()
{
    Super::PostLoad();
    bTiledImageDirty = true;

    if (mRasterBlock)
    {
        InitRasterBlock();
    }
}

TArray<::ULIS::FEvent>
UOdysseyAnimationCellImageRaster::RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FOdysseyInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList)
{
    UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(GetLayer());
    if (!layer || !layer->IsAlphaLocked())
        return iWaitList;

    //Apply AlphaLock
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlock();

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


void
UOdysseyAnimationCellImageRaster::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    InvalidateTiledImage(iRects);
    FOdysseySurfaceTexture2DEditable surface(GetRenderTexture(), GetRasterBlock()->GetBlock());
    surface.Invalidate(iRects);
    RenderingChanged(::ULISUtils::ToIntRects(iRects), true);
}

void
UOdysseyAnimationCellImageRaster::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
    InvalidateTiledImage(iRects);
    TSharedPtr<::ULIS::FBlock> block =  GetRasterBlock()->GetBlock();
    FOdysseySurfaceTexture2DEditable surface(GetRenderTexture(), block);
    surface.Invalidate(iRects);

    TArray<FIntRect> intRects = ::ULISUtils::ToIntRects(iRects);
    CopyBlockDataToTextureSource(block.Get(), GetRenderTexture(), intRects);

    RenderingChanged(::ULISUtils::ToIntRects(iRects), false);
}

FOdysseyMediaProvider
UOdysseyAnimationCellImageRaster::GetMediaProvider() const
{
    //Don't create a mediaRaster if there is an image render in use
    FScopeLock lock(&mImageRenderingMutex);

    TSharedPtr<FOdysseyMediaRaster> mediaRaster = MakeShared<FOdysseyMediaRaster>(GetRasterBlock());
    mMediaRaster = mediaRaster;
    FOdysseyMediaProvider mediaProvider;
    mediaProvider.Add(mediaRaster);
    return mediaProvider;
}

FCriticalSection*
UOdysseyAnimationCellImageRaster::GetImageRenderingMutex() const
{
    return &mImageRenderingMutex;
}

void
UOdysseyAnimationCellImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if( Ar.IsSaving() && !Ar.IsTransacting() && !Ar.IsCooking())
    {
        //FOdysseyAnimationCellImageRasterExport::Write( this, Ar );
    }

    if( Ar.IsLoading() && !Ar.IsTransacting() )
    {
        if (!FOdysseyAnimationCellImageRasterImport::Read( this, Ar ))
        {
        }
    }
}

void
UOdysseyAnimationCellImageRaster::OldSerialize(FArchive& Ar)
{
    Super::OldSerialize(Ar);

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImageRasterImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            mRasterBlock = MakeShared<FOdysseyRasterBlock>(this);
            Ar << *mRasterBlock;
        }
        InitRasterBlock(); //PostLoad is not called after OldSerialize, so we need to initialize the rasterblock here
    }
}

void
UOdysseyAnimationCellImageRaster::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);

    if (!SaveContext.IsCooking())
    {
        UpdateTiledImage();
    }
}

void
UOdysseyAnimationCellImageRaster::Clear()
{
    FScopedTransaction ScopedTransaction(LOCTEXT("animation-cell-raster.clear", "Clear"));

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlock();

    FOdysseyRasterBlockMutator mutator(rasterBlock);
    mutator.EditTilesFromRects(
        { ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) },
        [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
            ::ULIS::FEvent eventClear;
            ctx.Clear(*iBlock, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);
            return { eventClear };
        }
    );
    mutator.Commit();
}

#endif

#undef LOCTEXT_NAMESPACE
