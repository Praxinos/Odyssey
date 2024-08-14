// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRenderer.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyAnimation.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"

FOdysseyAnimationCellImageRasterImageRenderer::FOdysseyAnimationCellImageRasterImageRenderer(const UOdysseyAnimationCellImageRaster* iCell, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCell(iCell)
    , mBlock(nullptr)
{
    UOdysseyAnimation* animation = mCell->GetAnimation();

    FVector2D outOfPegsPan = mCell->OutOfPegs.Pan;
    float outOfPegsRotation = mCell->OutOfPegs.Rotation;
    float outOfPegsZoom = mCell->OutOfPegs.Zoom;

    if (mCell->IsOutOfPegs())
    {
        mOutOfPegsTransform = ::ULIS::FMat3F::MakeTranslationMatrix(animation->Width() / 2.f, animation->Height() / 2.f)
            * ::ULIS::FMat3F::MakeTranslationMatrix(outOfPegsPan.X, outOfPegsPan.Y)
            * ::ULIS::FMat3F::MakeRotationMatrix(FMath::DegreesToRadians(outOfPegsRotation))
            * ::ULIS::FMat3F::MakeScaleMatrix(outOfPegsZoom, outOfPegsZoom)
            * ::ULIS::FMat3F::MakeTranslationMatrix( animation->Width() / -2.f, animation->Height() / -2.f);
    }
}
    
void
FOdysseyAnimationCellImageRasterImageRenderer::Init()
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mCell->GetRasterBlock();
    if (rasterBlock)
        mBlock = rasterBlock->GetBlock();
}

void
FOdysseyAnimationCellImageRasterImageRenderer::Lock()
{
    mCell->GetImageRenderingMutex()->Lock();
}

void
FOdysseyAnimationCellImageRasterImageRenderer::Unlock()
{
    mCell->GetImageRenderingMutex()->Unlock();
}

bool
FOdysseyAnimationCellImageRasterImageRenderer::IsGameThreadOnly()
{
    return mCell->IsImageRenderingGameThreadOnly();
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageRasterImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{   
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationCellImageRasterImageRenderer::Blend);
    if (!mBlock)
        return iWaitList;

    FOdysseyImageRendererBlendParams params(iParams);
    if (GetRenderType() == IOdysseyImageRenderer::eRenderType::RenderOutOfPegs)
        params.mTransform = mOutOfPegsTransform;

    return ConvertAndBlend(mBlock, ::ULIS::FVec2I(0), params, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageRasterImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationCellImageRasterImageRenderer::Copy);
    if (!mBlock)
        return iWaitList;

    FOdysseyImageRendererCopyParams params(iParams);
    if (GetRenderType() == IOdysseyImageRenderer::eRenderType::RenderOutOfPegs)
        params.mTransform = mOutOfPegsTransform;

    return ConvertAndCopy(mBlock, ::ULIS::FVec2I(0), params, iWaitList);
}
