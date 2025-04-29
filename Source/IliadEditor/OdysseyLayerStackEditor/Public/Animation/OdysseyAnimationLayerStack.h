// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerStack.h"

#include "OdysseyAnimationLayerStack.generated.h"

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

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|LayerStack")
    UOdysseyAnimation* GetAnimation() const;

public:
    virtual FIntRect GetDefaultRenderRect() const override;

private:
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);
};
