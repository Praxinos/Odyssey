// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyRenderContext.h"

#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "OdysseyAnimation.h"

#if WITH_EDITOR
#include "Engine/Texture2D.h"
#include "Misc/App.h"
#include "OdysseyBlendShader.h"
#include "OdysseyTiledImage.h"
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
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
) const
{
#if WITH_EDITOR
    const_cast<UOdysseyAnimationCellImageRaster*>(this)->MigrateLegacyContent();
    return GetRasterContent().BuildRenderFunction(oRenderFunction, GetContentTransform(RenderContext));
#else
    return Super::BuildRenderPipelineInternal(oRenderFunction, iParents, RenderContext);
#endif
}

#if WITH_EDITOR
const UOdysseyTiledImage*
UOdysseyAnimationCellImageRaster::GetTiledImage() const
{
    return TiledImage;
}

FOdysseyRasterContent&
UOdysseyAnimationCellImageRaster::GetRasterContent() const
{
    if (!mRasterContent)
    {
        UOdysseyAnimationCellImageRaster* MutableThis = const_cast<UOdysseyAnimationCellImageRaster*>(this);
        // As the former render texture, 8-bit raster blocks of cells hold sRGB encoded colors; the tiles hold them decoded.
        mRasterContent = MakeUnique<FOdysseyRasterContent>(*MutableThis, MutableThis->GetRenderingId(), MutableThis->TiledImage,
            [MutableThis]()
            {
                FOdysseyRasterContent::FDescription Description;
                if (MutableThis->GetAnimation())
                {
                    Description.Size = MutableThis->GetDefaultRenderRect().Size();
                    Description.Format = MutableThis->GetRasterFormat();
                    Description.bSRGB = true;
                }
                return Description;
            },
            [MutableThis](const TArray<FIntRect>& Rects, bool bInteractive)
            {
                MutableThis->OnContentChanged(Rects, bInteractive);
            },
            FOdysseyRasterBlock::FPostProcess::CreateUObject(MutableThis, &UOdysseyAnimationCellImageRaster::RasterBlockPostProcess));
    }
    return *mRasterContent;
}

::ULIS::eFormat
UOdysseyAnimationCellImageRaster::GetRasterFormat() const
{
    UOdysseyAnimation* Animation = GetAnimation();
    return Animation && Animation->GetFormat() == EOdysseyAnimationFormat::RGBAF ? ::ULIS::Format_RGBAF : ::ULIS::Format_BGRA8;
}

void
UOdysseyAnimationCellImageRaster::OnContentChanged(const TArray<FIntRect>& Rects, bool bInteractive)
{
    if (Rects.IsEmpty())
    {
        RenderingChanged({ GetDefaultRenderRect() }, bInteractive);
        return;
    }
    RenderingChanged(Rects, bInteractive);
}

void
UOdysseyAnimationCellImageRaster::MigrateLegacyContent()
{
    UTexture2D* LegacyTexture = GetExistingRenderTexture();
    if (HasAnyFlags(RF_ClassDefaultObject) || (!mLegacyRasterBlock && !LegacyTexture) || !FApp::CanEverRender())
    {
        return;
    }

    TSharedPtr<::ULIS::FBlock> Pixels;
    if (mLegacyRasterBlock)
    {
        Pixels = mLegacyRasterBlock->GetBlock();
    }
    else if (LegacyTexture->Source.IsValid())
    {
        Pixels = MakeShareable(NewBlockFromUTextureData(LegacyTexture, GetRasterFormat()));
    }

    // Keep the legacy data when it cannot be migrated, e.g. while the cell is not yet in an animation.
    if (Pixels && !GetRasterContent().SetContent(*Pixels))
    {
        return;
    }
    mLegacyRasterBlock.Reset();
    DiscardRenderTexture();
}

void
UOdysseyAnimationCellImageRaster::InitTexture()
{
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyAnimationCellImageRaster::GetRasterBlock() const
{
    const_cast<UOdysseyAnimationCellImageRaster*>(this)->MigrateLegacyContent();
    return GetRasterContent().GetRasterBlock();
}

void
UOdysseyAnimationCellImageRaster::PostLoad()
{
    Super::PostLoad();
    MigrateLegacyContent();
}

void
UOdysseyAnimationCellImageRaster::BeginDestroy()
{
    mRasterContent.Reset();
    Super::BeginDestroy();
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
            mLegacyRasterBlock = MakeShared<FOdysseyRasterBlock>(this);
            Ar << *mLegacyRasterBlock;
        }
        // PostLoad is not called after OldSerialize: the legacy block is migrated on first use.
    }
}

void
UOdysseyAnimationCellImageRaster::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);

    // Content still held by a legacy render texture must reach the tiled image before saving.
    MigrateLegacyContent();
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
