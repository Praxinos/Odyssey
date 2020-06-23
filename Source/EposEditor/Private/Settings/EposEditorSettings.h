// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
//#include "Board/BoardSequence.h"
//#include "Shot/ShotSequence.h"
#include "EposEditorSettings.generated.h"

//---

USTRUCT()
struct FBoardTrackSettings
{
	GENERATED_BODY()

	UPROPERTY(config, EditAnywhere, Category=BoardTrackSettings)
	int NewEmptyBoardSectionLength;
};

USTRUCT()
struct FBoardSettings
{
	GENERATED_BODY()

	UPROPERTY(config, EditAnywhere, Category=BoardTrack)
	FBoardTrackSettings BoardTrackSettings;
};

//---

USTRUCT()
struct FShotSettings
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

	/** Specifies Shot stuff. */
	UPROPERTY(config, EditAnywhere, Category=Shot)
	FShotSettings ShotSettings;
};
