// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "OdysseyRenderContext.h"

struct ODYSSEYANIMATION_API FOdysseyAnimationRenderContext : IOdysseyRenderContextExtension
{
    FOdysseyAnimationRenderContext();
    virtual ~FOdysseyAnimationRenderContext() override;
    static FName TypeName();
    virtual TUniquePtr<IOdysseyRenderContextExtension> Clone() const override;
    TOptional<FFrameNumber> Frame;
};
