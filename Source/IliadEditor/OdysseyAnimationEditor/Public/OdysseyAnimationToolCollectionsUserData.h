// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "UObject/SoftObjectPath.h"

#include "OdysseyAnimationToolCollectionsUserData.generated.h"

class UOdysseyAnimation;

/**
 * @brief Editor-only tool collections of a UOdysseyAnimation, attached to it as an asset user data
 *
 * Its class lives in an editor module, so it is never cooked.
 */
UCLASS()
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationToolCollectionsUserData
    : public UAssetUserData
{
    GENERATED_BODY()

public:
    /**
     * @brief Returns the tool collections user data of an animation, creating it if needed
     *
     * Moves the legacy tool collection paths still stored on the animation into the user data.
     *
     * @param iAnimation The animation owning the user data
     * @return The tool collections user data of the animation, or nullptr if iAnimation is null
     */
    static UOdysseyAnimationToolCollectionsUserData* FindOrCreate(const UOdysseyAnimation* iAnimation);

private:
    /** Moves the legacy tool collection paths stored on the owning animation into this user data */
    void MigrateLegacyData();

public:
    /** Paths to the tool collections used with the animation, kept as paths so this module does not depend on the painter module */
    UPROPERTY()
    TArray<FSoftObjectPath> ToolCollections;
};
