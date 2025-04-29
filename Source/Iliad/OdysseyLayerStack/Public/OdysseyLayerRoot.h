// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"

#include "OdysseyLayerRoot.generated.h"

UCLASS(NotBlueprintType, HideDropdown)
class UOdysseyLayerRoot
    : public UOdysseyLayer
{
    GENERATED_BODY()

public:
    UOdysseyLayerRoot();

public:
    TSharedPtr<FOdysseyTextureRenderer> BuildTextureRenderer(FFrameNumber iFrame, TMap<const IOdysseyTextureRenderingAbility*, FGuid>* iIds = nullptr) const override;
};
