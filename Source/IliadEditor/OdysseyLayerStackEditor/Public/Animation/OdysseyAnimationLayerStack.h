// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerStack.h"
#include "Misc/OdysseyHandle.h"
#include "OdysseyAnimationCellSelection.h"

#include "OdysseyAnimationLayerStack.generated.h"

class UOdysseyAnimationLayer;
class FOdysseyAnimationProxy;

UCLASS(config=EditorPerProjectUserSettings, PerObjectConfig, BlueprintType)
class ODYSSEYLAYERSTACKEDITOR_API UOdysseyAnimationLayerStack
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
    virtual FInt32Range GetFrameRange() const override;

public:
    virtual TArray<FIntRect> GetRenderingRects() const override;
    TSharedRef<FOdysseyAnimationCellSelection> GetCellSelection() const;

private:
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);

private:
    TSharedRef<FOdysseyAnimationCellSelection> mCellSelection;

public:
    UPROPERTY(config)
    float TimelineSplitterPosition = 0.2f; //TODO: Move To Editor Only class
};
