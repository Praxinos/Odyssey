// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImageRenderer.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"

static FCriticalSection mEngineMutex;

FOdysseyAnimationCellImageVectorImageRenderer::FOdysseyAnimationCellImageVectorImageRenderer(TSharedRef<const FOdysseyAnimationCellImageVector> iCell, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCell(iCell)
    , mBlock(nullptr)
{
}
    
void
FOdysseyAnimationCellImageVectorImageRenderer::Init()
{
    TSharedPtr<FOdysseyVectorBlock> vectorBlock = mCell->GetVectorBlock();

    {   
        FScopeLock renderLock(&mEngineMutex);
        vectorBlock->Render();
    }

    if (vectorBlock)
        mBlock = vectorBlock->GetBlock();
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageVectorImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{   
    if (!mBlock)
        return iWaitList;

    return ConvertAndBlend(mBlock, ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageVectorImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    return ConvertAndCopy(mBlock, ioBlock, iRects, iPos, iWaitList);
}

void
FOdysseyAnimationCellImageVectorImageRenderer::Lock()
{
    mCell->GetImageRenderingMutex()->Lock();
}

void
FOdysseyAnimationCellImageVectorImageRenderer::Unlock()
{
    mCell->GetImageRenderingMutex()->Unlock();
}

bool
FOdysseyAnimationCellImageVectorImageRenderer::IsGameThreadOnly()
{
    return mCell->IsImageRenderingGameThreadOnly();
}
