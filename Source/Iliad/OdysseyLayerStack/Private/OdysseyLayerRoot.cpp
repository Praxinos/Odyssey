// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerRoot.h"

UOdysseyLayerRoot::UOdysseyLayerRoot()
{
    bCanHaveChildren = true;
}

TSharedPtr<FOdysseyTextureRenderer>
UOdysseyLayerRoot::BuildTextureRenderer(FFrameNumber iFrame, TMap<const IOdysseyTextureRenderingAbility*, FGuid>* iIds) const
{
    TSharedPtr<FOdysseyTextureRenderer> renderer = MakeShared<FOdysseyTextureRenderer>();
    for (const UOdysseyLayer* layer : Children)
    {
        renderer->Append(renderer->GetRootPassId(), *layer->BuildTextureRenderer(iFrame, iIds));
    }

    return renderer;
}
