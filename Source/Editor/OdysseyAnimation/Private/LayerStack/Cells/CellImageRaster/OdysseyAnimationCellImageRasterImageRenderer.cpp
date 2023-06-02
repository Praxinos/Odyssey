// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRenderer.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"

FOdysseyAnimationCellImageRasterImageRenderer::FOdysseyAnimationCellImageRasterImageRenderer(TSharedPtr<FOdysseyAnimationCellImageRaster> iCell, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mRasterBlock(iCell->GetRasterBlock())
{
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageRasterImageRenderer::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!ioBlock)
        return iWaitList;

    TSharedPtr<ULIS::FBlock> block = mRasterBlock->GetBlock();
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
