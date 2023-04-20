// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/OdysseyAnimationLayerImageRenderingAbility.h"

class ODYSSEYANIMATION_API FOdysseyAnimationLayerFolderImageRenderingAbility
    : public FOdysseyAnimationLayerImageRenderingAbility
{
public:
    FOdysseyAnimationLayerFolderImageRenderingAbility(UOdysseyAnimationLayerFolder* iLayerFolder);

public:
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
    virtual TArray<::ULIS::FEvent> RenderOverBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);

private:
    UOdysseyAnimationLayerFolder* mLayerFolder;
};