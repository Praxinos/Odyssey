// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRenderingAbility.h"

#include "Misc/OdysseyHandle.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"

FOdysseyAnimationCellImageRasterImageRenderingAbility::FOdysseyAnimationCellImageRasterImageRenderingAbility(TSharedPtr<FOdysseyAnimationCellImageRaster> iCellImageRaster)
    : mCellImageRaster(iCellImageRaster)
{
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
FOdysseyAnimationCellImageRasterImageRenderingAbility::RenderInBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
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
    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, block->Format(), iRect, iPos, iWaitList,
        [this, &block, &ctx](TSharedPtr<::ULIS::FBlock> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FEvent eventCopy = FULISEventBuilder().RetainBlock(block).Build();
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

    ctx.Flush();

    return eventConvertAndExecute;
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
