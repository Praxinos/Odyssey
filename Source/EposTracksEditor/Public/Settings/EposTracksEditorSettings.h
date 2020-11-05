// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ArrangeSectionsType.h"

#include "EposTracksEditorSettings.generated.h"

//---

USTRUCT()
struct FBoardTrackSettings
{
	GENERATED_BODY()

	/** Select the way to arrange board sections. */
	UPROPERTY(config, EditAnywhere, Category=BoardTrackSettings)
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
 * Epos Editor settings.
 */
UCLASS(config=EposTracks)
class EPOSTRACKSEDITOR_API UEposTracksEditorSettings
	: public UObject
{
	GENERATED_BODY()

public:

	UEposTracksEditorSettings(const FObjectInitializer& ObjectInitializer);
    
	/** Specifies Board track stuff. */
	UPROPERTY(config, EditAnywhere, Category=BoardTrack)
	FBoardTrackSettings BoardTrackSettings;

	/** Specifies Shot track stuff. */
	UPROPERTY(config, EditAnywhere, Category=ShotTrack)
	FShotTrackSettings ShotTrackSettings;
};
