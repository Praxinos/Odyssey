// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerImageRenderer.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"

FOdysseyAnimationCellImageStaggerImageRenderer::FOdysseyAnimationCellImageStaggerImageRenderer(TSharedRef<const FOdysseyAnimationCellImageStagger> iCell, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCellRenderer(nullptr)
{
    int cellFrame = INDEX_NONE;
    TSharedPtr<FOdysseyAnimationCell> cell = iCell->GetReferenceCellAtFrame(iFrame, &cellFrame);
    if (cell)
        mCellRenderer = cell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, cellFrame, iFilter);
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
