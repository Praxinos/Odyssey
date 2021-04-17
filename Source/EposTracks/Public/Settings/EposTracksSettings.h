// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ArrangeSectionsType.h"

#include "EposTracksSettings.generated.h"

//---

USTRUCT()
struct FBoardTrackSettings
{
    GENERATED_BODY()

public:
    /** Select the way to arrange board sections. */
    UPROPERTY(config, EditAnywhere, Category=BoardTrack)
    EArrangeSections ArrangeShots;
};

//---

USTRUCT()
struct FShotTrackSettings
{
    GENERATED_BODY()
};

//---

/**
 * Epos Tracks settings.
 */
UCLASS(config=Epos)
class EPOSTRACKS_API UEposTracksSettings
    : public UObject
{
    GENERATED_BODY()

public:
    UEposTracksSettings();

    /** Specifies Board track stuff. */
    UPROPERTY(config, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FBoardTrackSettings BoardTrackSettings;

    /** Specifies Shot track stuff. */
    UPROPERTY(config, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FShotTrackSettings ShotTrackSettings;
};
