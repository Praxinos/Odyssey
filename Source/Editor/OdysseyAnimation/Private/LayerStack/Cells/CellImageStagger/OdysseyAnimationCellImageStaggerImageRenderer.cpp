// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerImageRenderer.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"

FOdysseyAnimationCellImageStaggerImageRenderer::FOdysseyAnimationCellImageStaggerImageRenderer( const UOdysseyAnimationCellImageStagger* iCell, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCellRenderer(nullptr)
{
    int staggerFrame = iCell->GetReferenceFrameAtFrame(iFrame);
    UOdysseyAnimationCell* cell =  iCell->GetLayer()->GetCellAtFrame(staggerFrame);

    if (cell)
    {
        int cellFrame = iFrame - cell->GetFrameRange().GetLowerBoundValue();
        mCellRenderer = cell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, cellFrame, iFilter);
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
