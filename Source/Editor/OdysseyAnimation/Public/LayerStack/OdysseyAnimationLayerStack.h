// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerStack.h"
#include "Misc/OdysseyHandle.h"

#include <ULIS>

#include "OdysseyAnimationLayerStack.generated.h"

class UOdysseyAnimationLayer;
class FOdysseyAnimationCellSelection;

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayerStack
    : public UOdysseyLayerStack
{
    GENERATED_BODY()

public:
    UOdysseyAnimationLayerStack();

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|LayerStack")
    UOdysseyAnimation* GetAnimation() const;

public:
    //Animation Specific
    /**
     * @brief Will return the actual frame range
     *
     * @return FInt32Range
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    FInt32Range GetFrameRange() const;

public:
    virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
    TSharedRef<FOdysseyAnimationCellSelection> GetCellSelection() const;

private:
    TSharedRef<FOdysseyAnimationCellSelection> mCellSelection;
};