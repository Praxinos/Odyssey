// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "EposEditorSettings.generated.h"

//---

USTRUCT()
struct FBoardSettings
{
	GENERATED_BODY()
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
