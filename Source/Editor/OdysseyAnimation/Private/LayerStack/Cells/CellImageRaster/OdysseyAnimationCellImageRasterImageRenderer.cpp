// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRenderer.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"

FOdysseyAnimationCellImageRasterImageRenderer::FOdysseyAnimationCellImageRasterImageRenderer(TSharedRef<const FOdysseyAnimationCellImageRaster> iCell, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCell(iCell)
    , mBlock(nullptr)
{
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
FOdysseyAnimationCellImageRasterImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{   
    if (!mBlock)
        return iWaitList;

    return ConvertAndBlend(mBlock, ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageRasterImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    return ConvertAndCopy(mBlock, ioBlock, iRects, iPos, iWaitList);
}
