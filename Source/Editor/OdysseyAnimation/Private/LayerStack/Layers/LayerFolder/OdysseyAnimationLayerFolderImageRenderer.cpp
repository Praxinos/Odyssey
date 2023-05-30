// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolderImageRenderer.h"

FOdysseyAnimationLayerFolderImageRenderer::FOdysseyAnimationLayerFolderImageRenderer(UOdysseyAnimationLayerFolder* iLayer, int iFrame)
    : FOdysseyAnimationLayerImageRenderer(iLayer, iFrame)
    , mBlendMode(::ULIS::eBlendMode(iLayer->BlendMode))
    , mOpacity(iLayer->Opacity)
{
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerFolderImageRenderer::RenderOverBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!ioBlock)
        return iWaitList;

    ::ULIS::eFormat format = ioBlock->Format();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    TArray<::ULIS::FEvent> events;
    for (int i = 0; i < iRects.Num(); i++)
    {
        //Generate the folder block, which is all children layers blended together
        TArray<::ULIS::FEvent> folderBlockEvent;
        TSharedPtr<::ULIS::FBlock> folderBlock = RenderInNewBlock(format, iRects[i], folderBlockEvent);
        if(!folderBlock)
            continue;

        folderBlockEvent.Append(iWaitList);
            
        //Convert the destination if needed and Blend the folderBlock
        TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, folderBlock->Format(), folderBlock->Rect(), iPos[i], folderBlockEvent,
            [this, &folderBlock, &ctx](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
            {
                //if ioBlock and mBlock use same format, Blend directly in ioBlock
                ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(folderBlock).RetainBlock(ioDest).Build();

                ctx.Blend(
                    *folderBlock,
                    *ioDest,
                    iRect,
                    iPos,
                    mBlendMode,
                    ::ULIS::Alpha_Normal,
                    mOpacity,
                    ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                    iWaitList.Num(),
                    iWaitList.GetData(),
                    &eventBlend
                );

                return { eventBlend };
            }
        );

        events.Append(eventConvertAndExecute);
    }

    ctx.Flush();

    return events;
}
