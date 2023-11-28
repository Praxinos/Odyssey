// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerStack.h"
#include "Misc/OdysseyHandle.h"
#include "OdysseyAnimationImageRenderingAbility.h"

#include <ULIS>

#include "OdysseyAnimationLayerStack.generated.h"

class UOdysseyAnimationLayer;

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayerStack
    : public UOdysseyLayerStack
    , public FOdysseyAnimationImageRenderingAbility
    
{
    GENERATED_BODY()

public:
    UOdysseyAnimationLayerStack();

public:
    UFUNCTION(BlueprintPure, Category="LayerStack")
    UOdysseyAnimation* GetAnimation() const;

public:
    //Animation Specific
    /**
     * @brief Will return the actual frame range 
     * 
     * @return FInt32Range 
     */
    FInt32Range GetFrameRange() const;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
};