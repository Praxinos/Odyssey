// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "StoryboardSettings.generated.h"

//---

UCLASS( config=Epos )
class UStoryboardSettings
    : public UObject
{
    GENERATED_BODY()

public:
    /** Storyboard name. */
    UPROPERTY( config, EditAnywhere, Category=Storyboard )
    FString StoryboardName { TEXT( "MyStoryboard" ) };

    /** Storyboard path. */
    UPROPERTY( config, EditAnywhere, Category=Storyboard, meta=(ContentDir) )
    FDirectoryPath StoryboardPath { TEXT( "/Game" ) };
};
