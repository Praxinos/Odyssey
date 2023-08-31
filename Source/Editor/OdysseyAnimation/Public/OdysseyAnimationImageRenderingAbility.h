// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImageRenderer.h"
#include "OdysseyImageRenderingAbility.h"

#include <ULIS>

class ODYSSEYANIMATION_API FOdysseyAnimationImageRenderingAbility
    : public FOdysseyImageRenderingAbility
{
public:
    /**
     * @brief Creates a renderer able to render an image at the specified frame
     * This renderer is made to always render the same rendering composition
     * For example : if you delete a layer, you should create a new renderer
     * but if you are just drawing on the layer, you can reuse the renderer
     */
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame = 0) const;

    /**
     * @brief Returns the full Render Image Id, eventually composed of underlying ids
     *
     * @return const FGuid&
     */
    virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex = 0) const;

    /**
     * @brief Preloads in memory everything needed to make RenderImage() as fast as possible
     *
     * @param iFrame
     */
    virtual TSharedPtr<IOdysseyHandle> PreloadImageRendering(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame = 0) const;

    virtual ::ULIS::eBlendMode GetImageRenderingBlendMode() const;

    virtual float GetImageRenderingOpacity() const;
};