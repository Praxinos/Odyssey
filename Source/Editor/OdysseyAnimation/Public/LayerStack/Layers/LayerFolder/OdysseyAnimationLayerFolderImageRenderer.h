// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/OdysseyAnimationLayerImageRenderer.h"

class ODYSSEYANIMATION_API FOdysseyAnimationLayerFolderImageRenderer
    : public FOdysseyAnimationLayerImageRenderer
{
public:
    FOdysseyAnimationLayerFolderImageRenderer(UOdysseyAnimationLayerFolder* iLayer, int iFrame, bool iThreadSafe);

public:
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

public:
	::ULIS::eBlendMode mBlendMode;
    float mOpacity;
};
