// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRenderer.h"
#include "ULISLoaderModule.h"
#include "OdysseyMediaRaster.h"

#define LOCTEXT_NAMESPACE "FOdysseyAnimationCellImageRaster"

TSharedPtr<FOdysseyAnimationCellImageRaster>
FOdysseyAnimationCellImageRaster::Create(UOdysseyAnimationLayerImageRaster* iLayer, int iWidth, int iHeight, ::ULIS::eFormat iFormat)
{
    TSharedPtr<FOdysseyAnimationCellImageRaster> cell = MakeShared<FOdysseyAnimationCellImageRaster>(iLayer);
    cell->Init(iWidth, iHeight, iFormat);
    return cell;
}

TSharedPtr<FOdysseyAnimationCellImageRaster>
FOdysseyAnimationCellImageRaster::Create(UOdysseyAnimationLayerImageRaster* iLayer, TSharedPtr<::ULIS::FBlock> iBlock)
{
    TSharedPtr<FOdysseyAnimationCellImageRaster> cell = MakeShared<FOdysseyAnimationCellImageRaster>(iLayer);
    cell->Init(iBlock);
    return cell;
}

TSharedPtr<FOdysseyRasterBlock>
FOdysseyAnimationCellImageRaster::GetRasterBlock() const
{
    return mRasterBlock;
}

const FName&
FOdysseyAnimationCellImageRaster::StaticType()
{
    static FName type = TEXT("FOdysseyAnimationCellImageRaster");
    return type;
}

FOdysseyAnimationCellImageRaster::~FOdysseyAnimationCellImageRaster()
{
    mRasterBlock->OnBlockChanged().RemoveAll(this);
    mRasterBlock->OnBlockCommited().RemoveAll(this);
    mRasterBlock->OnBlockPtrChanged().RemoveAll(this);
}

FOdysseyAnimationCellImageRaster::FOdysseyAnimationCellImageRaster(UOdysseyAnimationLayerImageRaster* iLayer)
    : mLayer(iLayer)
    , mRasterBlock(nullptr)

{
}

void
FOdysseyAnimationCellImageRaster::Init(int iWidth, int iHeight, ::ULIS::eFormat iFormat)
{
    mRasterBlock = MakeShared<FOdysseyRasterBlock>(mLayer);
    mRasterBlock->OnBlockChanged().AddRaw(this, &FOdysseyAnimationCellImageRaster::OnBlockChanged);
    mRasterBlock->OnBlockCommited().AddRaw(this, &FOdysseyAnimationCellImageRaster::OnBlockCommited);
    mRasterBlock->OnBlockPtrChanged().AddRaw(this, &FOdysseyAnimationCellImageRaster::OnBlockPtrChanged);
    mRasterBlock->PostProcess().BindRaw(this, &FOdysseyAnimationCellImageRaster::RasterBlockPostProcess);
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iWidth, iHeight, iFormat);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFormat);
    ctx.Clear(*block.Get());
    ctx.Finish();

    mRasterBlock->SetBlock(block);
}

void
FOdysseyAnimationCellImageRaster::Init(TSharedPtr<::ULIS::FBlock> iBlock)
{
    mRasterBlock = MakeShared<FOdysseyRasterBlock>(mLayer);
    mRasterBlock->OnBlockChanged().AddRaw(this, &FOdysseyAnimationCellImageRaster::OnBlockChanged);
    mRasterBlock->OnBlockCommited().AddRaw(this, &FOdysseyAnimationCellImageRaster::OnBlockCommited);
    mRasterBlock->OnBlockPtrChanged().AddRaw(this, &FOdysseyAnimationCellImageRaster::OnBlockPtrChanged);
    mRasterBlock->PostProcess().BindRaw(this, &FOdysseyAnimationCellImageRaster::RasterBlockPostProcess);
    mRasterBlock->SetBlock(iBlock);

}

const FName&
FOdysseyAnimationCellImageRaster::GetType() const
{
    return StaticType();
}

void
FOdysseyAnimationCellImageRaster::PostLoad()
{
    FOdysseyAnimationCell::PostLoad();
    mRasterBlock->PostProcess().Unbind();
    mRasterBlock->PostProcess().BindRaw(this, &FOdysseyAnimationCellImageRaster::RasterBlockPostProcess);
}

void
FOdysseyAnimationCellImageRaster::PostDuplicate()
{
    FOdysseyAnimationCell::PostDuplicate();
}

void
FOdysseyAnimationCellImageRaster::Serialize(FArchive& Ar)
{
    FOdysseyAnimationCell::Serialize(Ar);

    if ( Ar.IsLoading() )
    {
        mRasterBlock = MakeShared<FOdysseyRasterBlock>(mLayer);
    }
    
    Ar << *mRasterBlock;

    if ( Ar.IsLoading() )
    {
        mRasterBlock->OnBlockChanged().RemoveAll(this);
        mRasterBlock->OnBlockCommited().RemoveAll(this);
        mRasterBlock->OnBlockPtrChanged().RemoveAll(this);
        mRasterBlock->OnBlockChanged().AddRaw(this, &FOdysseyAnimationCellImageRaster::OnBlockChanged);
        mRasterBlock->OnBlockCommited().AddRaw(this, &FOdysseyAnimationCellImageRaster::OnBlockCommited);
        mRasterBlock->OnBlockPtrChanged().AddRaw(this, &FOdysseyAnimationCellImageRaster::OnBlockPtrChanged);
    }
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageRaster::RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FULISInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayer->IsAlphaLocked)
        return iWaitList;

    //Apply AlphaLock
    TArray<::ULIS::FEvent> events;
    TSharedPtr<::ULIS::FBlock> block = mRasterBlock->GetBlock();
    TArray<FIntPoint> invalidTiles = iInvalidMap.InvalidTiles();    
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mRasterBlock->GetFormat());
    for (const FIntPoint& invalidTile : invalidTiles)
    {
        TSharedPtr<::ULIS::FBlock> originalBlock = iOriginalBlocks[invalidTile];
        ::ULIS::FRectI rect = iInvalidMap.GetTileRect(invalidTile);
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

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationCellImageRaster::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    return MakeShared<FOdysseyAnimationCellImageRasterImageRenderer>(SharedThis(this), iFrame, iRenderType, GetImageRenderingRects());
}

TArray<FGuid>
FOdysseyAnimationCellImageRaster::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    return { GetImageRenderingId() };
}

TArray<::ULIS::FRectI>
FOdysseyAnimationCellImageRaster::GetImageRenderingRects() const
{
    if (!mRasterBlock)
        return {};

    return { ::ULIS::FRectI::FromXYWH(0, 0, mRasterBlock->GetWidth(), mRasterBlock->GetHeight()) };
}

void
FOdysseyAnimationCellImageRaster::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    ImageRenderingChanged(iRects);
}

void
FOdysseyAnimationCellImageRaster::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
    ImageRenderingCommited(iRects);
}

void
FOdysseyAnimationCellImageRaster::OnBlockPtrChanged()
{
    ImageRenderingChanged();
    ImageRenderingCommited();
}

FOdysseyMediaProvider
FOdysseyAnimationCellImageRaster::GetMediaProvider(uint32 iFrameIndex) const
{
    TSharedPtr<FOdysseyMediaRaster> mediaRaster = MakeShared<FOdysseyMediaRaster>(GetRasterBlock());
    FOdysseyMediaProvider mediaProvider;
    mediaProvider.Add(mediaRaster);
    return mediaProvider;
}

#undef LOCTEXT_NAMESPACE