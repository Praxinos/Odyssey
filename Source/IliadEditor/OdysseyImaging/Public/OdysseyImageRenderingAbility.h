// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyRenderingAbility.h"
#include "OdysseyImageRenderer.h"

class ODYSSEYIMAGING_API FOdysseyImageRenderingAbility
    : public IOdysseyRenderingAbility
{
public:
    /**
     * @brief Creates a renderer able to render an image at the specified frame
     * This renderer is made to always render the same rendering composition
     * For example : if you delete a layer, you should create a new renderer
     * but if you are just drawing on the layer, you can reuse the renderer
     */
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(EOdysseyRenderingType iRenderType, int iFrame = 0, FImageRendererFilter iFilter = FImageRendererFilter()) const;
};
