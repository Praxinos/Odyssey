// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "OdysseyRenderContext.h"

/**
 * The animation part of a render request: which frame to render.
 *
 * An animation renders Frame, in animation frames; frame 0 when unset.
 * Layers and cells read the frame of the request and rewrite it for their children, e.g. a layer passes the frame
 * relative to a cell, and a lighttable the frame of each of its keys.
 */
struct ODYSSEYANIMATION_API FOdysseyAnimationRenderContext : IOdysseyRenderContextExtension
{
    FOdysseyAnimationRenderContext();
    virtual ~FOdysseyAnimationRenderContext() override;
    static FName TypeName();
    virtual TUniquePtr<IOdysseyRenderContextExtension> Clone() const override;

    /** Returns a render context requesting one frame of an animation. */
    static FOdysseyRenderContext MakeForFrame(FFrameNumber InFrame);

    /** The frame to render; frame 0 when unset. */
    TOptional<FFrameNumber> Frame;
};
