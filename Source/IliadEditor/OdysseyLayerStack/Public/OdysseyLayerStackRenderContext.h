// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "OdysseyRenderContext.h"
#include "OdysseyBlendMode.h"

struct FOdysseyLighttable;

/**
 * Overrides of the properties of one layer for one render request, without changing the layer.
 * Unset values keep the properties of the layer. Part of the render override API: a request may set them, e.g. to render
 * a layer hidden or with another opacity.
 */
struct ODYSSEYLAYERSTACK_API FOdysseyLayerStackRenderOverride
{
    TOptional<bool> bVisible;
    TOptional<float> Opacity;
    TOptional<EOdysseyBlendMode> BlendMode;
    TOptional<bool> bInheritsAlpha;

    /** Replaces the lighttable of an animation layer. */
    TSharedPtr<const FOdysseyLighttable> Lighttable;

    /** When false, the override is ignored while the lighttable keys of the layer are rendered (see bLighttablePass). */
    bool bApplyToLighttable = false;
};

/**
 * The layer stack part of a render request (an extension of FOdysseyRenderContext), read on the game thread when the
 * render pipeline is built, and copied with the request.
 */
struct ODYSSEYLAYERSTACK_API FOdysseyLayerStackRenderContext : IOdysseyRenderContextExtension
{
    FOdysseyLayerStackRenderContext();
    virtual ~FOdysseyLayerStackRenderContext() override;

    static FName TypeName();

    virtual TUniquePtr<IOdysseyRenderContextExtension> Clone() const override;

    /** Returns the override of a layer, by its rendering id; null when none applies, e.g. during a lighttable pass. */
    const FOdysseyLayerStackRenderOverride* Find(const FGuid& Id) const;

    /** The overrides, by rendering id of the layer. */
    TMap<FGuid, FOdysseyLayerStackRenderOverride> Overrides;

    /** True while the lighttable keys are rendered: only overrides with bApplyToLighttable apply. */
    bool bLighttablePass = false;

    /** False to ignore the "display only the current layer" option of the stack, e.g. to render every layer at full opacity. */
    bool bUseDisplayOnlyCurrentLayer = true;
};
