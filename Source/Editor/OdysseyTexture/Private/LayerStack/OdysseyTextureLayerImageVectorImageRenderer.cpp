// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyTextureLayerImageVectorImageRenderer.h"
#include "OdysseyVectorBlock.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyLayerStack.h"
#include "OdysseyTextureLayerImageVector.h"

FOdysseyTextureLayerImageVectorImageRenderer::FOdysseyTextureLayerImageVectorImageRenderer(const UOdysseyTextureLayerImageVector* iLayer, TSharedPtr<FOdysseyVectorBlock> iVectorBlock, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mVectorBlock(iVectorBlock)
    , mBlock(nullptr)
    , mHUDBlock(nullptr)
    , mDrawingFlags(0)
    , mRenderHUD(false)
{
    UOdysseyLayerStack* layerStack = iLayer->GetLayerStack();
    if (!layerStack)
        return;

    mRenderHUD = layerStack->CurrentLayer.Get() == iLayer;

    // this is per-layer
    mDrawingFlags  = iLayer->IsColored   ? mDrawingFlags & (~FOdysseyVectorEngine::DRAWING_IGNORECOLOR )
                                         : mDrawingFlags | ( FOdysseyVectorEngine::DRAWING_IGNORECOLOR );
    // TEMP: this should be global, stored in PainterEditor. Hence this should be
    // changed when PainterEditor will be available and we can retrieve the shared flags.
    // Update: commented-out for now
    mDrawingFlags |= iLayer->IsWireframe ? mDrawingFlags | ( FOdysseyVectorEngine::DRAWING_WIREFRAME)
                                         : mDrawingFlags & (~FOdysseyVectorEngine::DRAWING_WIREFRAME);
}

void
FOdysseyTextureLayerImageVectorImageRenderer::Init()
{
    mBlock = mVectorBlock->GetBlock(mDrawingFlags);
    if (mRenderHUD)
        mHUDBlock = mVectorBlock->GetHUDBlock();

    mVectorBlock->Render(mDrawingFlags);
}

bool
FOdysseyTextureLayerImageVectorImageRenderer::IsGameThreadOnly()
{
    return false;
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerImageVectorImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    TArray<::ULIS::FEvent> events = ConvertAndBlend(mBlock, ::ULIS::FVec2I(0), iParams, iWaitList);

    if (!mHUDBlock)
        return events;

    FOdysseyImageRendererBlendParams hudParams = iParams;
    hudParams.mBlendMode = ::ULIS::Blend_Normal;
    hudParams.mOpacity = 1.f;

    return ConvertAndBlend(mHUDBlock, ::ULIS::FVec2I(0), hudParams, events);
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerImageVectorImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    return ConvertAndCopy(mBlock, ::ULIS::FVec2I(0), iParams, iWaitList);
}
