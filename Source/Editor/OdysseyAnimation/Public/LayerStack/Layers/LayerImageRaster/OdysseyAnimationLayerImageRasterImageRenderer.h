// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"

class ODYSSEYANIMATION_API FOdysseyAnimationLayerImageRasterImageRenderer
    : public IOdysseyImageRenderer
{
public:
    FOdysseyAnimationLayerImageRasterImageRenderer(UOdysseyAnimationLayerImageRaster* iLayer, int iFrame);

public:
    /**
     * @brief Returns the full rect that can be rendered
     * 
     * @return ::ULIS::FRect 
     */
    virtual TArray<::ULIS::FRectI> GetRects() const override;

    /**
     * @brief Renders over (by blending for example) the given block
     * By default does the same thing as RenderInBlock
     *
     * @param ioBlock
     * @param iRect
     * @param iPos
     * @param iWaitList
     * @return TArray<::ULIS::FEvent>
     */
    TArray<::ULIS::FEvent> RenderOverBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;

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
    TSharedPtr<IOdysseyImageRenderer> mCellRenderer;
    ::ULIS::eBlendMode mBlendMode;
    float mOpacity;
};
