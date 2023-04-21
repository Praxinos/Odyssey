// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Abilities/IOdysseyAnimationImageRenderingAbility.h"

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageRasterImageRenderingAbility
    : public IOdysseyAnimationImageRenderingAbility
{
public:
    virtual ~FOdysseyAnimationCellImageRasterImageRenderingAbility();
    FOdysseyAnimationCellImageRasterImageRenderingAbility(TSharedPtr<FOdysseyAnimationCellImageRaster> iCellImageRaster);

public:
    /**
     * @brief Returns the full rect that can be rendered
     * 
     * @return ::ULIS::FRect 
     */
    virtual TArray<::ULIS::FRectI> GetRects(int iFrame) const override;
    
    /**
     * @brief Renders an image in the given block
     * 
     * @param ioBlock 
     * @param iFrame 
     * @param iRect 
     * @param iPos 
     * @param iWaitList 
     * @return TArray<::ULIS::FEvent> 
     */
    virtual TArray<::ULIS::FEvent> RenderInBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;

    /**
     * @brief Returns the full Render Image Id, eventually composed of underlying ids
     * Order is important, it should go from top to bottom, depth first
     * example {this, child1, subchild1, subchild2, child2, child3}
     * 
     * @return const FGuid& 
     */
    virtual TArray<FGuid> GetComposition(int iFrameIndex) const override;

    /**
     * @brief Preloads in memory everything needed to make RenderImage() as fast as possible
     * 
     * @param iFrame 
     */
    virtual TSharedPtr<IOdysseyHandle> Preload(int iFrame) override;

private:
    void OnBlockChanged(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockCommited(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockPtrChanged();

private:
    TWeakPtr<FOdysseyAnimationCellImageRaster> mCellImageRaster;
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
};