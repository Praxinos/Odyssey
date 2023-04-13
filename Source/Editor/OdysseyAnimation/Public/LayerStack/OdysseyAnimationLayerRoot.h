// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationLayerRoot.generated.h"

UCLASS()
class ODYSSEYANIMATION_API UOdysseyAnimationLayerRoot
	: public UOdysseyAnimationLayer
{
    GENERATED_BODY()

public:
    UOdysseyAnimationLayerRoot();

protected:
    virtual void RenderImageChanged(const FOdysseyAnimationRenderImageId& iFrameId, const TArray<::ULIS::FRectI>& iRects) override;
};