// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"

#include "UObject/OdysseyObjectPropertyTracker.h"
#include "Misc/OdysseyHandle.h"
#include <ULIS>

#include "OdysseyAnimationLayer.generated.h"

class FOdysseyAnimationCellsContainer;

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayer
    : public UOdysseyLayer
    , public FOdysseyAnimationImageRenderingAbility
{
    GENERATED_BODY()

public:
    //Getters
    UOdysseyAnimation* GetAnimation() const;
    virtual FInt32Range GetFrameRange() const;
    virtual TSharedPtr<FOdysseyAnimationCellsContainer> GetCellsContainer() const { return nullptr; }
    
protected:
    //Property changes
    virtual void ChildrenChanged() override;
    virtual void IsActivatedChanged() override;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
};
