// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "StoryboardSettings.generated.h"

//---

UENUM()
enum class ELevelDestination: uint8
{
    /** Create a new empty level to manage the new storyboard.
      * (Usually, it's when you don't have to manage a 3D environement)
      */
    NewLevel        UMETA( DisplayName = "In a new level" ),

    /** Create a new empty sublevel (within the current opened level) to manage the new storyboard.
      * This option is recommanded for keeping all the actors in your current existing 3D environment separate from all the actors in your storyboard.
      */
    NewSublevel     UMETA( DisplayName = "In a new sublevel" ),

    /** Manage the new storyboard directly within the current opened level.
      * (Usually, the 2 previous options are always preferable)
      */
    CurrentLevel    UMETA( DisplayName = "In the current level" ),
};

UCLASS( config=Epos )
class UStoryboardSettings
    : public UObject
{
    GENERATED_BODY()

public:
    /** Storyboard name. */
    UPROPERTY( config, EditAnywhere, Category="Root Board", DisplayName="Name" )
    FString StoryboardName = TEXT( "MyStoryboard" );

    /** Storyboard path. */
    UPROPERTY( config, EditAnywhere, Category="Root Board", DisplayName="Path", meta=(ContentDir) )
    FDirectoryPath StoryboardPath = FDirectoryPath{ TEXT( "/Game" ) };

    /** New level destination. */
    UPROPERTY( config, EditAnywhere, Category = "Level", DisplayName = "Destination" )
    ELevelDestination LevelDestination = ELevelDestination::NewLevel;

    /** New level name. */
    UPROPERTY( config, EditAnywhere, Category = "Level", DisplayName = "Name" )
    FString LevelName = TEXT( "MyLevel" );

    /** New level path. */
    UPROPERTY( config, EditAnywhere, Category="Level", DisplayName="Path", meta=(ContentDir) )
    FDirectoryPath LevelPath = FDirectoryPath{ TEXT( "/Game" ) };
};
