// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCellImageRaster.h"

#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "OdysseyAnimation.h"

#if WITH_EDITOR
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
#include "UObject/ObjectSaveContext.h"
#include "OdysseyRasterBlockMutator.h"

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
    FFrameNumber iFrame,
    uint64 iType,
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents
) const
{
    return Super::BuildRenderPipelineInternal(iFrame, iType, oRenderFunction, iCanRenderFunction, iParents);
}

#if WITH_EDITOR
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
    FOdysseySurfaceTexture2DEditable surface(GetRenderTexture(), GetRasterBlock()->GetBlock());
    surface.Invalidate(iRects);
    RenderingChanged(::ULISUtils::ToIntRects(iRects), true);
}

void
UOdysseyAnimationCellImageRaster::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
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

    //InitTexture();
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
