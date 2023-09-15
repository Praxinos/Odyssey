// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImageRenderer.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"

static FCriticalSection mEngineMutex;

FOdysseyAnimationCellImageVectorImageRenderer::FOdysseyAnimationCellImageVectorImageRenderer(TSharedPtr<FOdysseyVectorBlock> iVectorBlock, bool iRenderHUD, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mVectorBlock(iVectorBlock)
    , mBlock(iVectorBlock->GetBlock())
    , mRenderHUD(iRenderHUD)
{
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageVectorImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{   
    if (!mBlock)
        return iWaitList;

    {   
        FScopeLock renderLock(&mEngineMutex);

        mVectorBlock->SetRenderHUD(mRenderHUD);
        mVectorBlock->Render();
    }
    

    return ConvertAndBlend(mBlock, ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageVectorImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    {   
        FScopeLock renderLock(&mEngineMutex);

        mVectorBlock->SetRenderHUD(mRenderHUD);
        mVectorBlock->Render(); //mIsColored ? 0 : FOdysseyVectorObject::DRAWING_IGNORECOLOR );
    }

    return ConvertAndCopy(mBlock, ioBlock, iRects, iPos, iWaitList);
}
