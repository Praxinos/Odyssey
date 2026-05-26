// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyLayerStack.h"

#include "OdysseyAnimationLayerStack.generated.h"

UCLASS(config=EditorPerProjectUserSettings, PerObjectConfig, BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayerStack
    : public UOdysseyLayerStack
{
    GENERATED_BODY()

public:
    UOdysseyAnimationLayerStack();

public:
    virtual void PostInitProperties() override;

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|LayerStack")
    UOdysseyAnimation* GetAnimation() const;

public:
    virtual UTextureRenderTarget2D* CreateRenderingRenderTarget() const override;
#if WITH_EDITOR
    virtual UTexture2D* CreateExportTexture(UObject* Outer, FName Name, EObjectFlags Flags = RF_NoFlags) override;
#endif
    virtual FIntRect GetDefaultRenderRect() const override;

private:
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);
};
