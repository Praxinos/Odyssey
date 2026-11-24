// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#include "OdysseyAnimationRenderContext.h"

FOdysseyAnimationRenderContext::FOdysseyAnimationRenderContext() = default;
FOdysseyAnimationRenderContext::~FOdysseyAnimationRenderContext() = default;

FName FOdysseyAnimationRenderContext::TypeName()
{
    return TEXT("OdysseyAnimation");
}

FOdysseyRenderContext FOdysseyAnimationRenderContext::MakeForFrame(FFrameNumber InFrame)
{
    FOdysseyRenderContext RenderContext;
    RenderContext.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = InFrame;
    return RenderContext;
}

TUniquePtr<IOdysseyRenderContextExtension> FOdysseyAnimationRenderContext::Clone() const
{
    TUniquePtr<FOdysseyAnimationRenderContext> Copy = MakeUnique<FOdysseyAnimationRenderContext>();
    Copy->Frame = Frame;
    return Copy;
}
