// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"

#include "OdysseyAnimationRenderingOverride.h"
#include "OdysseyPaletteReferencer.h"

#include "OdysseyAnimationLayerStackUserData.generated.h"

class UOdysseyAnimation;
class UOdysseyAnimationLayerStack;
class UOdysseyPalette;

/**
 * @brief Editor-only layer stack of a UOdysseyAnimation, attached to it as an asset user data
 *
 * Overrides the animation rendering with its layer stack, so that edits are displayed live in editor.
 * Use OdysseyAnimationEditor::GetLayerStack() to access the layer stack of an animation.
 * Its class lives in an editor module, so it is never cooked: the runtime only plays the baked frames.
 */
UCLASS(BlueprintType)
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationLayerStackUserData
    : public UAssetUserData
    , public IOdysseyAnimationRenderingOverride
    , public IOdysseyPaletteReferencer
{
    GENERATED_BODY()

public:
    /**
     * @brief Returns the layer stack user data of an animation, creating it if needed
     *
     * Moves the legacy layer stack still stored on the animation into the user data,
     * and creates an empty layer stack if the animation has none.
     *
     * @param iAnimation The animation owning the user data
     * @return The layer stack user data of the animation, or nullptr if iAnimation is null
     */
    static UOdysseyAnimationLayerStackUserData* FindOrCreate(const UOdysseyAnimation* iAnimation);

public:
    /** Returns the animation owning this user data */
    UOdysseyAnimation* GetAnimation() const;

    //~ Begin IOdysseyAnimationRenderingOverride Interface
    virtual const IOdysseyTextureRenderingAbility* GetAnimationRenderingOverride() const override;
    //~ End IOdysseyAnimationRenderingOverride Interface

private:
    //~ Begin IOdysseyPaletteReferencer Interface
    virtual void OnRefreshReferencedPalette(UOdysseyPalette* iPalette) override;
    //~ End IOdysseyPaletteReferencer Interface

    /** Moves the legacy layer stack stored on the owning animation into this user data */
    void MigrateLegacyData();

    /** Creates an empty layer stack if this user data has none */
    void CreateLayerStackIfNeeded();

public:
    /** Layer stack holding the editable layers and cells of the animation */
    UPROPERTY(BlueprintReadOnly, Category="Odyssey|LayerStack")
    TObjectPtr<UOdysseyAnimationLayerStack> LayerStack;
};
