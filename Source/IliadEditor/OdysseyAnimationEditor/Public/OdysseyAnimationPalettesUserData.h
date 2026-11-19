// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"

#include "OdysseyAnimationPalettesUserData.generated.h"

class UOdysseyAnimation;
class UOdysseyPaletteSet;

/**
 * @brief Editor-only palette sets of a UOdysseyAnimation, attached to it as an asset user data
 *
 * Its class lives in an editor module, so it is never cooked.
 */
UCLASS()
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationPalettesUserData
    : public UAssetUserData
{
    GENERATED_BODY()

public:
    /**
     * @brief Returns the palettes user data of an animation, creating it if needed
     *
     * Moves the legacy palette sets still stored on the animation into the user data.
     *
     * @param iAnimation The animation owning the user data
     * @return The palettes user data of the animation, or nullptr if iAnimation is null
     */
    static UOdysseyAnimationPalettesUserData* FindOrCreate(const UOdysseyAnimation* iAnimation);

private:
    /** Moves the legacy palette sets stored on the owning animation into this user data */
    void MigrateLegacyData();

public:
    /** Palette sets used by the vector layers of the animation */
    UPROPERTY()
    TArray<UOdysseyPaletteSet*> Palettes;
};
