// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/LightTable/OdysseyAnimationLightTableImageRenderer.h"

FOdysseyAnimationLightTableImageRenderer::FOdysseyAnimationLightTableImageRenderer(TSharedPtr<FOdysseyAnimationLightTable> iLightTable, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
{
    TSharedPtr<IOdysseyAnimationImageRenderingAbility> ability = iLightTable->GetSourceLayer()->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (ability)
    {
        const TArray<FOdysseyAnimationLightTable::FKeyData>& keysData = iLightTable->GetKeysData();
        for (int i = 0; i < keysData.Num(); i++)
        {
            if (!iLightTable->GetKeyIsActivated(i))
                continue;

            FFrameData data;
            data.mOpacity = iLightTable->GetKeyOpacity(i);
            data.mDisplayMode = iLightTable->GetKeyDisplayMode(i);
            data.mColor = iLightTable->GetKeyColor(i);
            data.mRenderer = ability->BuildRenderer(iFrame + iLightTable->GetKeyOffset(i), IOdysseyImageRenderer::eRenderType::Render);
            mFramesData.Add(data);
        }
    }
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLightTableImageRenderer::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ioBlock->Format());
    TArray<::ULIS::FEvent> rectEvents;
    for (int i = 0; i < iRects.Num(); i++)
    {
        TArray<::ULIS::FEvent> frameWaitList = iWaitList;
        for (const FFrameData& frameData : mFramesData)
        {
            const ::ULIS::FRectI& rect = iRects[i];
            const ::ULIS::FVec2I& pos = iPos[i];
            TArray<::ULIS::FEvent> events;
            TSharedPtr<::ULIS::FBlock> block = frameData.mRenderer->RenderInNewBlock(ioBlock->Format(), rect, events);
            if (!block)
                continue;

            switch(frameData.mDisplayMode)
            {
                case FOdysseyAnimationLightTable::eFrameDisplayMode::kDefault:
                break;

                case FOdysseyAnimationLightTable::eFrameDisplayMode::kColor:
                {
                    ::ULIS::FEvent eventDisplayMode = FULISEventBuilder()
                    .RetainBlock(block)
                    .Build();

                    ctx.FillPreserveAlpha(
                        *block,
                        frameData.mColor,
                        block->Rect(),
                        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                        events.Num(),
                        events.GetData(),
                        &eventDisplayMode
                    );

                    events = { eventDisplayMode };
                }
                break;

                case FOdysseyAnimationLightTable::eFrameDisplayMode::kTint:
                {
                    //TODO:
                }
                break;
            }

            frameWaitList.Append(events);

            TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, block->Format(), block->Rect(), pos, frameWaitList,
                [&frameData, &block, &ctx](TSharedPtr<::ULIS::FBlock> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
                {
                    ::ULIS::FEvent eventBlend = FULISEventBuilder()
                        .RetainBlock(block)
                        .RetainBlock(ioDest)
                        .Build();

                    ctx.Blend(
                        *block,
                        *ioDest,
                        iRect,
                        iPos,
                        ::ULIS::eBlendMode::Blend_Normal,
                        ::ULIS::eAlphaMode::Alpha_Normal,
                        frameData.mOpacity,
                        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                        iWaitList.Num(),
                        iWaitList.GetData(),
                        &eventBlend
                    );
                    return { eventBlend };
                }
            );

            frameWaitList = eventConvertAndExecute;
        }        
            
        rectEvents.Append(frameWaitList);
    }

    return rectEvents;
}
