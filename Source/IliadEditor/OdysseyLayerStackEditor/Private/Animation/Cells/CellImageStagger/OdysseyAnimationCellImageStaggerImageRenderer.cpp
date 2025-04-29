// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationCellImageStaggerImageRenderer.h"
#include "OdysseyAnimationCellImageStagger.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationLayer.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"

FOdysseyAnimationCellImageStaggerImageRenderer::FOdysseyAnimationCellImageStaggerImageRenderer( const UOdysseyAnimationCellImageStagger* iCell, int iFrame, EOdysseyRenderingType iRenderType, const TArray<FIntRect>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCellRenderer(nullptr)
{
    int staggerFrame = iCell->GetReferenceFrameAtFrame(iFrame);
    UOdysseyAnimationCell* cell =  iCell->GetLayer()->GetCellAtFrame(staggerFrame);

    if (cell)
    {
        int cellFrame = iFrame - cell->GetFrameRange().GetLowerBoundValue();
        mCellRenderer = cell->BuildImageRenderer(EOdysseyRenderingType::Render, cellFrame, iFilter);
    }
}

void
FOdysseyAnimationCellImageStaggerImageRenderer::Init()
{
    if (mCellRenderer)
        mCellRenderer->Init();
}

bool
FOdysseyAnimationCellImageStaggerImageRenderer::IsGameThreadOnly()
{
    if (!mCellRenderer)
        return false;

    return mCellRenderer->IsGameThreadOnly();
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageStaggerImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mCellRenderer)
        return iWaitList;

    return mCellRenderer->Blend(iParams, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageStaggerImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mCellRenderer)
        return iWaitList;

    return mCellRenderer->Copy(iParams, iWaitList);
}
