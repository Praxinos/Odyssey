// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerImageRasterImageRenderer.h"

#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyRasterBlock.h"

FOdysseyTextureLayerImageRasterImageRenderer::FOdysseyTextureLayerImageRasterImageRenderer(const UOdysseyTextureLayerImageRaster* iLayer, EOdysseyRenderingType iRenderType, const TArray<FIntRect>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mRasterBlock(iLayer->GetRasterBlock())
    , mBlock(nullptr)
{
}

void
FOdysseyTextureLayerImageRasterImageRenderer::Init()
{
    if (mRasterBlock)
        mBlock = mRasterBlock->GetBlock();
}

bool
FOdysseyTextureLayerImageRasterImageRenderer::IsGameThreadOnly()
{
    return false;
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerImageRasterImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    return ConvertAndBlend(mBlock, ::ULIS::FVec2I(0), iParams, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerImageRasterImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    return ConvertAndCopy(mBlock, ::ULIS::FVec2I(0), iParams, iWaitList);
}
