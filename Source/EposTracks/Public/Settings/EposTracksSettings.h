// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
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
    EArrangeSections ArrangeSections { EArrangeSections::OnOneRow };
};

//---

/**
 * Epos Tracks settings.
 */
UCLASS(config=Epos, meta=(DisplayName="Epos Tracks"))
class EPOSTRACKS_API UEposTracksSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

    //~ UDeveloperSettings Interface
    virtual FName GetContainerName() const override;
    virtual FName GetCategoryName() const override;
    //~

public:
    /** Specifies Board track stuff. */
    UPROPERTY(config, EditAnywhere, Category=Settings, meta=(ShowOnlyInnerProperties))
    FBoardTrackSettings BoardTrackSettings;
};
