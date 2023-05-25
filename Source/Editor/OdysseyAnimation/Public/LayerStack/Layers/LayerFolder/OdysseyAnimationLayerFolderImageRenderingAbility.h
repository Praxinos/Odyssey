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
     * @brief Creates a renderer able to render an image at the specified frame
     * This renderer is made to always render the same rendering composition
     * For example : if you delete a layer, you should create a new renderer
     * but if you are just drawing on the layer, you can reuse the renderer
     */
    virtual TSharedPtr<IOdysseyImageRenderer> BuildRenderer(int iFrame) const override;
    
private:
    UOdysseyAnimationLayerFolder* mLayerFolder;
};