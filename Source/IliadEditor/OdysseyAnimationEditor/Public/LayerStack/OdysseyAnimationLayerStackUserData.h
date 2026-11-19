// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "UObject/SoftObjectPath.h"

#include "OdysseyAnimationEditorData.h"
#include "OdysseyPaletteReferencer.h"

#include "OdysseyAnimationLayerStackUserData.generated.h"

class UOdysseyAnimation;
class UOdysseyAnimationLayerStack;
class UOdysseyPalette;
class UOdysseyPaletteSet;

/**
 * @brief Editor-only content of a UOdysseyAnimation, attached to it as an asset user data
 *
 * Holds the layer stack, the palettes and the tool collections of the animation.
 * Its class lives in an editor module, so it is never cooked: the runtime only plays the baked frames.
 */
UCLASS(BlueprintType)
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationLayerStackUserData
    : public UAssetUserData
    , public IOdysseyAnimationEditorData
    , public IOdysseyPaletteReferencer
{
    GENERATED_BODY()

public:
    /**
     * @brief Returns the editor data of an animation, creating it if needed
     *
     * Moves the legacy editor content still stored on the animation into the editor data,
     * and creates an empty layer stack if the animation has none.
     *
     * @param iAnimation The animation owning the editor data
     * @return The editor data of the animation, or nullptr if iAnimation is null
     */
    static UOdysseyAnimationLayerStackUserData* FindOrCreate(const UOdysseyAnimation* iAnimation);

    /**
     * @brief Returns the layer stack of an animation, creating its editor data if needed
     *
     * @param iAnimation The animation owning the layer stack
     * @return The layer stack of the animation, or nullptr if iAnimation is null
     */
    static UOdysseyAnimationLayerStack* GetLayerStack(const UOdysseyAnimation* iAnimation);

public:
    /** Returns the animation owning this editor data */
    UOdysseyAnimation* GetAnimation() const;

    //~ Begin IOdysseyAnimationEditorData Interface
    virtual const IOdysseyTextureRenderingAbility* GetAnimationRenderingSource() const override;
    //~ End IOdysseyAnimationEditorData Interface

private:
    //~ Begin IOdysseyPaletteReferencer Interface
    virtual void OnRefreshReferencedPalette(UOdysseyPalette* iPalette) override;
    //~ End IOdysseyPaletteReferencer Interface

    /** Moves the legacy editor content stored on the owning animation into this editor data */
    void MigrateLegacyData();

    /** Creates an empty layer stack if this editor data has none */
    void CreateLayerStackIfNeeded();

public:
    /** Layer stack holding the editable layers and cells of the animation */
    UPROPERTY(BlueprintReadOnly, Category="Odyssey|LayerStack")
    TObjectPtr<UOdysseyAnimationLayerStack> LayerStack;

    /** Palette sets used by the vector layers of the animation */
    UPROPERTY()
    TArray<UOdysseyPaletteSet*> Palettes;

    /** Paths to the tool collections used with the animation, kept as paths so this module does not depend on the painter module */
    UPROPERTY()
    TArray<FSoftObjectPath> ToolCollections;
};
