// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyLayerStack.h"

#include "OdysseyAnimationLayerStack.generated.h"

/**
 * The layers of an animation, edited in the painter editor.
 *
 * Owned by the UOdysseyAnimationLayerStackUserData of the UOdysseyAnimation (asset user data, editor only), which renders
 * the animation in the editor through IOdysseyAnimationRenderingOverride; reach it with OdysseyAnimationEditor::GetLayerStack().
 * At runtime, the animation plays the frames baked from this stack.
 * Layers are blended with encoded colors unless UOdysseyAnimation::bLinearCompositing is set (see GetContentColorConversion()).
 */
UCLASS(config=EditorPerProjectUserSettings, PerObjectConfig, BlueprintType)
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationLayerStack
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
    virtual UTexture2D* CreateExportTexture( const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory ) override;
#endif
    virtual FIntRect GetDefaultRenderRect() const override;

    /**
     * The views display the encoded colors of the linear colors an animation outputs:
     * the stack renders linear colors, a layer renders linear colors when the animation blends linear colors.
     */
    virtual EOdysseyColorConversion GetDisplayColorConversion(const UOdysseyLayer* RenderedLayer) const override;

protected:
    /** Decodes the blended colors when the animation blends sRGB encoded colors: an animation outputs linear colors. */
    virtual EOdysseyColorConversion GetContentColorConversion(const FOdysseyRenderContext* RenderContext) const override;

private:
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);
};
