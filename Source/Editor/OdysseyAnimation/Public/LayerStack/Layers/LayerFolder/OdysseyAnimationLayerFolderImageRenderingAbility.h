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
     * 
     * iThreadSafe, defines if the created renderer will be used in an other thread than the main thread.
     * In that case, the renderer will be made so that it always renders exactly the same image all the time
     * and you will have to recreate the renderer even if you're are just drawing on the layer
     */
    virtual TSharedPtr<IOdysseyImageRenderer> BuildRenderer(int iFrame, bool iThreadSafe) const override;
    
private:
    UOdysseyAnimationLayerFolder* mLayerFolder;
};