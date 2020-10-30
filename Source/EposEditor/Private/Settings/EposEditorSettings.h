// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
//#include "Board/BoardSequence.h"
//#include "Shot/ShotSequence.h"
#include "ArrangeSections.h"
#include "EposEditorSettings.generated.h"

//---

USTRUCT()
struct FBoardSettings
{
	GENERATED_BODY()
};

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
struct FShotSettings
{
    GENERATED_BODY()
};

USTRUCT()
struct FShotTrackSettings
{
    GENERATED_BODY()
};

//---

/**
 * Epos Editor settings.
 */
UCLASS(config=Epos)
class UEposEditorSettings
	: public UObject
{
	GENERATED_BODY()

public:

	UEposEditorSettings(const FObjectInitializer& ObjectInitializer);
    
	/** Specifies Board stuff. */
	UPROPERTY(config, EditAnywhere, Category=Board)
	FBoardSettings BoardSettings;
    
	/** Specifies Board track stuff. */
	UPROPERTY(config, EditAnywhere, Category=BoardTrack)
	FBoardTrackSettings BoardTrackSettings;

	/** Specifies Shot stuff. */
	UPROPERTY(config, EditAnywhere, Category=Shot)
	FShotSettings ShotSettings;

	/** Specifies Shot track stuff. */
	UPROPERTY(config, EditAnywhere, Category=ShotTrack)
	FShotTrackSettings ShotTrackSettings;
};
