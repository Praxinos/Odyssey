// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageRasterImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyAnimationCellImageRasterImageRenderer(TSharedPtr<FOdysseyAnimationCellImageRaster> iCell, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects);

public:
    /**
     * @brief Renders in (without blending for example) the given block
     * 
     * @param ioBlock 
     * @param iFrame 
     * @param iRect 
     * @param iPos 
     * @param iWaitList 
     * @return TArray<::ULIS::FEvent> 
     */
    virtual TArray<::ULIS::FEvent> RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;    

public:
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
};
