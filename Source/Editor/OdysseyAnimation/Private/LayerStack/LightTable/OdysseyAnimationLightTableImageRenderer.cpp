// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/LightTable/OdysseyAnimationLightTableImageRenderer.h"

FOdysseyAnimationLightTableImageRenderer::FOdysseyAnimationLightTableImageRenderer(TSharedRef<const FOdysseyAnimationLightTable> iLightTable, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
{

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = iLightTable->GetSourceLayer()->GetCellsContainer();
    int currentCellIndex = cellsContainer->GetCellIndexAtFrame(iFrame);
    if (currentCellIndex == INDEX_NONE)
        return;

    const TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = cellsContainer->GetCells();
    if (currentCellIndex == INDEX_NONE)
        return;

    const TMap<int, FOdysseyAnimationLightTable::FKeyData>& keysData = iLightTable->GetKeysData();
    for (int i = -1; i >= -iLightTable->GetRange(); i--)
    {
        int cellIndex = currentCellIndex + i;
        if (cellIndex < 0 || cellIndex >= cells.Num())
            continue;

        TSharedPtr<FOdysseyAnimationCell> cell = cells[cellIndex];

        if (!keysData[i].mIsActivated)
            continue;

        int cellFirstFrame = cellsContainer->GetCellFrame(cell);

        FFrameData data;
        data.mOpacity = keysData[i].mOpacity;
        data.mRenderer = iLightTable->GetSourceLayer()->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, cellFirstFrame, iFilter);
        data.mColor = iLightTable->GetKeyColor(i);
        mFramesData.Add(data);
    }

    for (int i = 1; i <= iLightTable->GetRange(); i++)
    {
        int cellIndex = currentCellIndex + i;
        if (cellIndex < 0 || cellIndex >= cells.Num())
            continue;

        TSharedPtr<FOdysseyAnimationCell> cell = cells[cellIndex];

        if (!keysData[i].mIsActivated)
            continue;

        int cellFirstFrame = cellsContainer->GetCellFrame(cell);

        FFrameData data;
        data.mOpacity = keysData[i].mOpacity;
        data.mRenderer = iLightTable->GetSourceLayer()->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, cellFirstFrame, iFilter);
        data.mColor = iLightTable->GetKeyColor(i);
        mFramesData.Add(data);
    }

    mDisplayMode = iLightTable->GetKeyDisplayMode();
}
    
void
FOdysseyAnimationLightTableImageRenderer::Init()
{
    for (const FFrameData& frameData : mFramesData)
    {
        frameData.mRenderer->Init();
    }
}

bool
FOdysseyAnimationLightTableImageRenderer::IsGameThreadOnly()
{
    for (const FFrameData& frameData : mFramesData)
    {
        if (frameData.mRenderer->IsGameThreadOnly())
            return true;
    }
    return false;
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLightTableImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{   
    switch(mDisplayMode)
    {
        case FOdysseyAnimationLightTable::eFrameDisplayMode::kColored:
        {
            TArray<::ULIS::FEvent> events;
            for (int i = 0; i < iRects.Num(); i++)
            {
                const ::ULIS::FRectI& rect = iRects[i];
                const ::ULIS::FVec2I& pos = iPos[i];
                //TSharedPtr<::ULIS::FBlock> colorblock = MakeShared<::ULIS::FBlock>(rect.w, rect.h, ioBlock->Format());
                TSharedPtr<::ULIS::FBlock> greyblock = MakeShared<::ULIS::FBlock>(rect.w, rect.h, ::ULIS::Format_GAF);
                TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(rect.w, rect.h, ioBlock->Format());
                ::ULIS::FRectI blockRect = block->Rect();
                ::ULIS::FVec2I blockPos(0);
                
                
                TArray<::ULIS::FEvent> lastEvent = iWaitList;
                for (const FFrameData& frameData : mFramesData)
                {
                    TArray<::ULIS::FEvent> clearEvents = Clear(block, { blockRect }, { blockPos }, lastEvent);
                    TArray<::ULIS::FEvent> rendererBlendEvents = frameData.mRenderer->Copy(block, { rect }, { blockPos }, clearEvents);

                    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());

                    ::ULIS::FColor colorFull = frameData.mColor;
                    colorFull.SetAlphaF(1.f);

                    ctx.Finish();
                    double start = FPlatformTime::Seconds();

                    ::ULIS::FEvent eventConvertGrey = FULISEventBuilder().RetainBlock(block).Build();
                    ctx.ConvertFormat(
                          *block
                        , *greyblock
                        , ::ULIS::FRectI::Auto
                        , ::ULIS::FVec2I( 0 )
                        , ::ULIS::FSchedulePolicy::MultiScanlines
                        , rendererBlendEvents.Num()
                        , rendererBlendEvents.GetData()
                        , &eventConvertGrey
                    );
                    ctx.Finish();

                    double end = FPlatformTime::Seconds();
                    UE_LOG(LogTemp, Warning, TEXT("ctx.ConvertFormat1 in %f seconds."), end-start);
                    start = FPlatformTime::Seconds();

                    ::ULIS::FEvent eventFilter = FULISEventBuilder().RetainBlock(block).Build();

                    ctx.FilterInPlace(
                        [contrast = 1.f - frameData.mColor.AlphaF()]( ::ULIS::FPixel& iPixel, uint64 iNumPixels )
                        {
                            for (int i = 0; i < iNumPixels; i++, iPixel.Next())
                            {
                                float value = iPixel.GreyF() * contrast;
                                iPixel.SetGreyF(value);
                            }
                        }
                        , *greyblock
                        , ::ULIS::FRectI::Auto
                        , ::ULIS::FSchedulePolicy::MultiScanlines
                        , 1
                        , &eventConvertGrey
                        , &eventFilter
                    );

                    ctx.Finish();
                    
                    end = FPlatformTime::Seconds();
                    UE_LOG(LogTemp, Warning, TEXT("ctx.FilterInto in %f seconds."), end-start);
                    start = FPlatformTime::Seconds();

                    ::ULIS::FEvent eventConvert2 = FULISEventBuilder().RetainBlock(block).Build();
                    ctx.ConvertFormat(
                          *greyblock
                        , *block
                        , ::ULIS::FRectI::Auto
                        , ::ULIS::FVec2I( 0 )
                        , ::ULIS::FSchedulePolicy::MultiScanlines
                        , 1
                        , &eventFilter
                        , &eventConvert2
                    );
                    ctx.Finish();

                    end = FPlatformTime::Seconds();
                    UE_LOG(LogTemp, Warning, TEXT("ctx.ConvertFormat2 in %f seconds."), end-start);
                    start = FPlatformTime::Seconds();

                    ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(block).Build();
                    ctx.BlendColor(
                        colorFull,
                        *block,
                        ::ULIS::FRectI::Auto,
                        ::ULIS::Blend_Screen,
                        ::ULIS::Alpha_Back,
                        1.f,
                        ::ULIS::FSchedulePolicy::MultiScanlines,
                        1,
                        &eventConvert2,
                        &eventBlend
                    );

                    //This is the algo to Tint instead of just Fill Color
                    //We don't use it because a Luminosity blend is way to slow (~100ms)
                    //This is not usable, let's 
                    /* ::ULIS::FEvent eventColorize = FULISEventBuilder().RetainBlock(colorblock).Build();
                    ::ULIS::FColor colorFull = frameData.mColor;
                    colorFull.SetAlphaF(1.f);
                    ctx.Fill(
                        *colorblock,
                        colorFull,
                        ::ULIS::FRectI::Auto,
                        ::ULIS::FSchedulePolicy::MultiScanlines,
                        rendererBlendEvents.Num(),
                        rendererBlendEvents.GetData(),
                        &eventColorize
                    );

                    ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(block).RetainBlock(colorblock).Build();
                    ctx.Blend(
                        *block,
                        *colorblock,
                        ::ULIS::FRectI::Auto,
                        ::ULIS::FVec2I(0),
                        ::ULIS::Blend_Luminosity,
                        ::ULIS::Alpha_Top,
                        1.f - frameData.mColor.AlphaF(),
                        ::ULIS::FSchedulePolicy::MultiScanlines,
                        1,
                        &eventColorize,
                        &eventBlend
                    );

                    ::ULIS::FEvent eventBlend2 = FULISEventBuilder().RetainBlock(block).RetainBlock(colorblock).Build();
                    ctx.Blend(
                        *block,
                        *colorblock,
                        ::ULIS::FRectI::Auto,
                        ::ULIS::FVec2I(0),
                        ::ULIS::Blend_Back,
                        ::ULIS::Alpha_Top,
                        1.f,
                        ::ULIS::FSchedulePolicy::MultiScanlines,
                        1,
                        &eventBlend,
                        &eventBlend2
                    ); */

                    lastEvent = ConvertAndBlend(block, ioBlock, ::ULIS::Blend_Normal, frameData.mOpacity, { blockRect }, { pos }, {eventBlend});
                }

                events.Append(lastEvent);
            }
            return events;
        }
        break;

        case FOdysseyAnimationLightTable::eFrameDisplayMode::kTint:
        {

        }
        break;
            
        case FOdysseyAnimationLightTable::eFrameDisplayMode::kDefault:
        {
            TArray<::ULIS::FEvent> lastEvent = iWaitList;
            for (const FFrameData& frameData : mFramesData)
            {
                lastEvent = frameData.mRenderer->Blend(ioBlock, ::ULIS::Blend_Normal, frameData.mOpacity, iRects, iPos, lastEvent);
            }
            return lastEvent;
        }
        break;
    }

    return iWaitList;
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLightTableImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FEvent> clearEvents = Clear(ioBlock, iRects, iPos, iWaitList);
    TArray<::ULIS::FEvent> lastEvent = clearEvents;
    for (const FFrameData& frameData : mFramesData)
    {
        lastEvent = frameData.mRenderer->Blend(ioBlock, ::ULIS::Blend_Normal, frameData.mOpacity, iRects, iPos, lastEvent);
    }
    return lastEvent;
}
