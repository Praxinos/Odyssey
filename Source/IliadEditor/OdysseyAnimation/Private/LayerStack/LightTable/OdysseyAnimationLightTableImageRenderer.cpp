// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/LightTable/OdysseyAnimationLightTableImageRenderer.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "ULISLoaderModule.h"
#include "ULISEventBuilder.h"

FOdysseyAnimationLightTableImageRenderer::FOdysseyAnimationLightTableImageRenderer(const UOdysseyAnimationLayer* iLayer, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
{
    UOdysseyAnimation* animation = iLayer->GetAnimation();

    UOdysseyAnimationCell* cell = iLayer->GetCellAtFrame(iFrame);
    if (!cell)
        return;

    for (int i = 9; i >= 0; i--)
    {
        if (iLayer->Lighttable.PreviousKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->IndexInLayer - i - 1;
            if (keyCellIndex >= 0 && keyCellIndex < iLayer->GetCells().Num())
            {
                UOdysseyAnimationCell* keyCell = iLayer->GetCells()[keyCellIndex];
                if (!keyCell)
                    continue;

                FVector2D outOfPegsPan = keyCell->OutOfPegs.Pan;
                float outOfPegsRotation = keyCell->OutOfPegs.Rotation;
                float outOfPegsZoom = keyCell->OutOfPegs.Zoom;

                ::ULIS::FMat3F oopTransform = ::ULIS::FMat3F::MakeTranslationMatrix(animation->GetWidth() / 2.f, animation->GetHeight() / 2.f)
                * ::ULIS::FMat3F::MakeTranslationMatrix(outOfPegsPan.X, outOfPegsPan.Y)
                * ::ULIS::FMat3F::MakeRotationMatrix(FMath::DegreesToRadians(outOfPegsRotation))
                * ::ULIS::FMat3F::MakeScaleMatrix(outOfPegsZoom / 100.f, outOfPegsZoom / 100.f)
                * ::ULIS::FMat3F::MakeTranslationMatrix( animation->GetWidth() / -2.f, animation->GetHeight() / -2.f);

                FFrameData data;
                data.mOpacity = iLayer->Lighttable.PreviousKeys[i].Opacity / 100.f;
                data.mRenderer = keyCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0, iFilter);
                data.mColor = ::ULIS::FColor::FromRGBAF(
                    iLayer->Lighttable.PreviousKeysColor.R,
                    iLayer->Lighttable.PreviousKeysColor.G,
                    iLayer->Lighttable.PreviousKeysColor.B,
                    iLayer->Lighttable.PreviousKeysColor.A
                );
                data.mContrast = iLayer->Lighttable.PreviousKeysContrast / 100.f;
                data.mOutOfPegsTransform = oopTransform;
                mFramesData.Add(data);
            }
        }

        if (iLayer->Lighttable.NextKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->IndexInLayer + i + 1;
            if (keyCellIndex >= 0 && keyCellIndex < iLayer->GetCells().Num())
            {
                UOdysseyAnimationCell* keyCell = iLayer->GetCells()[keyCellIndex];
                if (!keyCell)
                    continue;

                FVector2D outOfPegsPan = keyCell->OutOfPegs.Pan;
                float outOfPegsRotation = keyCell->OutOfPegs.Rotation;
                float outOfPegsZoom = keyCell->OutOfPegs.Zoom;

                ::ULIS::FMat3F oopTransform = ::ULIS::FMat3F::MakeTranslationMatrix(animation->GetWidth() / 2.f, animation->GetHeight() / 2.f)
                * ::ULIS::FMat3F::MakeTranslationMatrix(outOfPegsPan.X, outOfPegsPan.Y)
                * ::ULIS::FMat3F::MakeRotationMatrix(FMath::DegreesToRadians(outOfPegsRotation))
                * ::ULIS::FMat3F::MakeScaleMatrix(outOfPegsZoom / 100.f, outOfPegsZoom / 100.f)
                * ::ULIS::FMat3F::MakeTranslationMatrix( animation->GetWidth() / -2.f, animation->GetHeight() / -2.f);

                FFrameData data;
                data.mOpacity = iLayer->Lighttable.NextKeys[i].Opacity / 100.f;
                data.mRenderer = keyCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0, iFilter);
                data.mColor = ::ULIS::FColor::FromRGBAF(
                    iLayer->Lighttable.NextKeysColor.R,
                    iLayer->Lighttable.NextKeysColor.G,
                    iLayer->Lighttable.NextKeysColor.B,
                    iLayer->Lighttable.NextKeysColor.A
                );
                data.mContrast = iLayer->Lighttable.NextKeysContrast / 100.f;
                data.mOutOfPegsTransform = oopTransform;
                mFramesData.Add(data);
            }
        }
    }
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
FOdysseyAnimationLightTableImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationLightTableImageRenderer::Blend);
    TArray<::ULIS::FEvent> events;
    for (const ::ULIS::FRectI& rect : iParams.mRects)
    {
        TSharedPtr<::ULIS::FBlock> greyblock = MakeShared<::ULIS::FBlock>(rect.w, rect.h, ::ULIS::Format_GAF);
        TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(rect.w, rect.h, iParams.mBlock->Format());
        ::ULIS::FRectI blockRect = block->Rect();
        ::ULIS::FVec2I blockPos(iParams.mPos.x + rect.x, iParams.mPos.y + rect.y);


        TArray<::ULIS::FEvent> lastEvent = iWaitList;
        for (const FFrameData& frameData : mFramesData)
        {
            TArray<::ULIS::FEvent> clearEvents = Clear(block, { blockRect }, lastEvent);

            FOdysseyImageRendererCopyParams frameParams(iParams);
            frameParams.mBlock = block;
            frameParams.mRects = { block->Rect() };
            frameParams.mPos = blockPos;
            frameParams.mTransform = frameData.mOutOfPegsTransform;

            TArray<::ULIS::FEvent> rendererBlendEvents = frameData.mRenderer->Copy(frameParams, clearEvents);

            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());

            ::ULIS::FColor colorFull = frameData.mColor;
            colorFull.SetAlphaF(1.f);

            ::ULIS::FEvent eventConvertGrey = FULISEventBuilder().RetainBlock(block).RetainBlock(greyblock).Build();
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

            ::ULIS::FEvent eventFilter = FULISEventBuilder().RetainBlock(greyblock).Build();

            ctx.FilterInPlace(
                [contrast = frameData.mContrast]( ::ULIS::FPixel& iPixel, uint64 iNumPixels )
                {
                    for (int i = 0; i < iNumPixels; i++, iPixel.Next())
                    {
                        float value = FMath::Clamp(iPixel.GreyF() * contrast * contrast * 4, 0.f, 1.0f); //contrast * contrast * 4 to make a progressively more powerful contrast (slider goes from 0-100, but effective contrast goes from 0-400 approx)
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

            ::ULIS::FEvent eventConvert2 = FULISEventBuilder().RetainBlock(greyblock).RetainBlock(block).Build();
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

            FOdysseyImageRendererBlendParams blendParams(iParams);
            blendParams.mBlendMode = ::ULIS::Blend_Normal;
            blendParams.mOpacity = frameData.mOpacity;
            blendParams.mRects = { rect };
            blendParams.mTransform = ::ULIS::FMat3F();

            lastEvent = ConvertAndBlend(block, blockPos, blendParams, {eventBlend});
        }

        events.Append(lastEvent);
    }
    return events;
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLightTableImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    FOdysseyImageRendererBlendParams params(iParams);

    return Blend(params, iWaitList);
}
