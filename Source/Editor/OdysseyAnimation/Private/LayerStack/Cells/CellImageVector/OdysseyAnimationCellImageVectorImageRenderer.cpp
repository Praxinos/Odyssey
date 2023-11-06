// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImageRenderer.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"

static FCriticalSection mEngineMutex;

FOdysseyAnimationCellImageVectorImageRenderer::FOdysseyAnimationCellImageVectorImageRenderer(TSharedRef<const FOdysseyAnimationCellImageVector> iCell, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCell(iCell)
    , mBlock(nullptr)
    , mHUDBlock(nullptr)
    , mRenderHUD(false)
    , mDrawingFlags(0)
{
    /* TEMPORARY FOR DISPLAYING TOOLS HUD */
    UOdysseyAnimationLayerImageVector* layer = Cast<UOdysseyAnimationLayerImageVector>(mCell->GetLayer());
    if (layer)
    {
        UOdysseyAnimation* animation = layer->GetAnimation();
        UOdysseyLayerStack* layerStack = layer->GetLayerStack();
        if (animation && layerStack)
        {
            TSharedPtr<FOdysseyAnimationCell> cell = layer->GetCellsContainer()->GetCellAtFrame(animation->CurrentFrame);
            int frame = layer->GetCellsContainer()->GetCellFrameAtFrame(animation->CurrentFrame);

            mRenderHUD = cell == mCell && frame == iFrame && layerStack->CurrentLayer.Get() == layer;
        }

        // this is per-layer
        mDrawingFlags = layer->IsColored   ? mDrawingFlags & (~FOdysseyVectorEngine::DRAWING_IGNORECOLOR)
                                           : mDrawingFlags |   FOdysseyVectorEngine::DRAWING_IGNORECOLOR;
        // TEMP: this should be global, stored in PainterEditor. Hence this should be
        // changed when PainterEditor will be available and we can retrieve the shared flags.
        // Update: commented-out for now
        //mDrawingFlags = layer->IsWireframe ? mDrawingFlags & (~FOdysseyVectorEngine::DRAWING_WIREFRAME)
        //                                   : mDrawingFlags |   FOdysseyVectorEngine::DRAWING_WIREFRAME;

    }
}
    
void
FOdysseyAnimationCellImageVectorImageRenderer::Init()
{
    TSharedPtr<FOdysseyVectorBlock> vectorBlock = mCell->GetVectorBlock();


    if (vectorBlock)
    {
        mBlock = vectorBlock->GetBlock( mDrawingFlags ); //Store block before rendering to avoid looking twice for the block in cache
        /* TEMPORARY FOR DISPLAYING TOOLS HUD */
        if (mRenderHUD)
            mHUDBlock = vectorBlock->GetHUDBlock();
    }

    {   
        FScopeLock renderLock(&mEngineMutex);
        vectorBlock->Render( mDrawingFlags );
    }
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageVectorImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{   
    if (!mBlock)
        return iWaitList;

    TArray<::ULIS::FEvent> events = ConvertAndBlend(mBlock, ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);
    
    if (!mHUDBlock)
        return events;

    return ConvertAndBlend(mHUDBlock, ioBlock, ::ULIS::Blend_Normal, 1.f, iRects, iPos, events);
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
