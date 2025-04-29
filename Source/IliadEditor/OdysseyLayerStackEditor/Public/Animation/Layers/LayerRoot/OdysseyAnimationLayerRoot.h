// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationLayerRoot.generated.h"

UCLASS(NotBlueprintType, HideDropdown)
class ODYSSEYLAYERSTACKEDITOR_API UOdysseyAnimationLayerRoot
    : public UOdysseyAnimationLayer
{
    GENERATED_BODY()

public:
    UOdysseyAnimationLayerRoot();

public:
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void RenderToTexture(FCanvas* iCanvas, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntRect& iDstRect) const override;

public:
    //TSharedPtr<FOdysseyAnimationProxy> GetProxy() const;

private:
    //TSharedPtr<FOdysseyAnimationProxy> mProxy; //Move To LayerStack
};
