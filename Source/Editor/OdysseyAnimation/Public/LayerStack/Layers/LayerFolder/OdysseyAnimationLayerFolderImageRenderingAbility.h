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
    //TODO: Move GetBlendMode and GetOpacity() in their own ability, so that the one using the ability will be the one calling OnChanged, etc...
    // Like BlendParamsAbility->OnChanged() => parentRenderingAbility->OnChanged
    virtual ::ULIS::eBlendMode GetBlendMode() const override;
    virtual float GetOpacity() const override;

private:
    UOdysseyAnimationLayerFolder* mLayerFolder;
};