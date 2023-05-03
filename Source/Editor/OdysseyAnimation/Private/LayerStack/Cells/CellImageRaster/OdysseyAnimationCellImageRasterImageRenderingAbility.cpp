// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRenderingAbility.h"

#include "Misc/OdysseyHandle.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"



FOdysseyAnimationCellImageRasterImageRenderingAbility::~FOdysseyAnimationCellImageRasterImageRenderingAbility()
{
    mRasterBlock->OnBlockChanged().RemoveAll(this);
    mRasterBlock->OnBlockCommited().RemoveAll(this);
    mRasterBlock->OnBlockPtrChanged().RemoveAll(this);

    mRasterBlock = nullptr;
}

FOdysseyAnimationCellImageRasterImageRenderingAbility::FOdysseyAnimationCellImageRasterImageRenderingAbility(TSharedPtr<FOdysseyAnimationCellImageRaster> iCellImageRaster)
    : mCellImageRaster(iCellImageRaster)
    , mRasterBlock(iCellImageRaster->GetRasterBlock())
{
    mRasterBlock->OnBlockChanged().AddRaw(this, &FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockChanged);
    mRasterBlock->OnBlockCommited().AddRaw(this, &FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockCommited);
    mRasterBlock->OnBlockPtrChanged().AddRaw(this, &FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockPtrChanged);
}

TArray<::ULIS::FRectI>
FOdysseyAnimationCellImageRasterImageRenderingAbility::GetRects(int iFrame) const
{
    TSharedPtr<FOdysseyAnimationCellImageRaster> cellImageRaster = mCellImageRaster.Pin();
    if (!cellImageRaster)
        return {};

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = cellImageRaster->GetRasterBlock();
    if (!rasterBlock)
        return {};

    return { ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) };
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageRasterImageRenderingAbility::RenderInBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    TSharedPtr<FOdysseyAnimationCellImageRaster> cellImageRaster = mCellImageRaster.Pin();
    if (!cellImageRaster)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = cellImageRaster->GetRasterBlock();
    if (!rasterBlock)
        return iWaitList;

    TSharedPtr<ULIS::FBlock> block = rasterBlock->GetBlock();
    if (!block)
        return iWaitList;
    
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());

    TArray<::ULIS::FEvent> events;
    for (int i = 0; i < iRects.Num(); i++)
    {
        TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, block->Format(), iRects[i], iPos[i], iWaitList,
            [this, &block, &ctx](TSharedPtr<::ULIS::FBlock> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
            {
                ::ULIS::FEvent eventCopy = FULISEventBuilder().RetainBlock(block).RetainBlock(ioDest).Build();
                ctx.Copy(
                    *block,
                    *ioDest,
                    iRect,
                    iPos,
                    ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                    iWaitList.Num(),
                    iWaitList.GetData(),
                    &eventCopy
                );
                return { eventCopy };
            }
        );
        events.Append(eventConvertAndExecute);
    }

    ctx.Flush();

    return events;
}

TArray<FGuid>
FOdysseyAnimationCellImageRasterImageRenderingAbility::GetComposition(int iFrameIndex) const
{
    return { GetId() };
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationCellImageRasterImageRenderingAbility::Preload(int iFrame)
{
    TSharedPtr<FOdysseyAnimationCellImageRaster> cellImageRaster = mCellImageRaster.Pin();
    if (!cellImageRaster)
        return nullptr;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = cellImageRaster->GetRasterBlock();
    if (!rasterBlock)
        return nullptr;

    TArray<TSharedPtr<IOdysseyHandle>> handles = { rasterBlock->Preload() };

    return MakeShared<FOdysseyHandleContainer>(handles);
}

void
FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    OnChanged().Broadcast(GetId(), iRects);
}

void
FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
    OnCommited().Broadcast(GetId(), iRects);
}

void
FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockPtrChanged()
{
    OnChanged().Broadcast(GetId(), GetRects(0));
    OnCommited().Broadcast(GetId(), GetRects(0));
}