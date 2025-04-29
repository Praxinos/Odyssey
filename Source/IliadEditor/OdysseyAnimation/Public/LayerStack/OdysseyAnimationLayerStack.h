// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerStack.h"
#include "Misc/OdysseyHandle.h"
#include "LayerStack/Cells/OdysseyAnimationCellSelection.h"

#include "OdysseyAnimationLayerStack.generated.h"

class UOdysseyAnimationLayer;
class FOdysseyAnimationProxy;

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayerStack
    : public UOdysseyLayerStack
{
    GENERATED_BODY()

public:
    UOdysseyAnimationLayerStack();

public:
    virtual void PostInitProperties() override;

public:
    virtual int GetWidth() const override;
    virtual int GetHeight() const override;
    virtual ::ULIS::eFormat  GetFormat() const override;

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
    virtual TArray<FIntRect> GetRenderingRects() const override;
    TSharedRef<FOdysseyAnimationCellSelection> GetCellSelection() const;

public:
    virtual void RenderToTextureFromRects(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects) const override;

private:
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);

private:
    TSharedRef<FOdysseyAnimationCellSelection> mCellSelection;
};
