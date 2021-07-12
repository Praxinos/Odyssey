// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "CineCameraComponent.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/Object.h"

#include "EposSequenceEditorSettings.generated.h"

//---

USTRUCT()
struct FBoardSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(config, EditAnywhere, Category=Board)
    FFrameRate DefaultTickFrameRate { 24000, 1 };

    UPROPERTY(config, EditAnywhere, Category=Board)
    FFrameRate DefaultDisplayFrameRate { 24, 1 };

    /** The default duration for new board sequences in seconds. */
    UPROPERTY(config, EditAnywhere, Category=Board, meta=(ClampMin=0.00001f, Units=s))
    float DefaultDuration { 3 };

};

//---

USTRUCT()
struct FShotSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(config, EditAnywhere, Category=Shot)
    FFrameRate DefaultTickFrameRate { 24000, 1 };

    UPROPERTY(config, EditAnywhere, Category=Shot)
    FFrameRate DefaultDisplayFrameRate { 24, 1 };

    /** The default duration for new shot sequences in seconds. */
    UPROPERTY(config, EditAnywhere, Category=Shot, meta=(ClampMin=0.00001f, Units=s))
    float DefaultDuration { 3 };
};

//---

/**
 * Epos Sequence Editor settings.
 */
UCLASS(config=Epos, meta=(DisplayName="Epos Sequence Editor"))
class UEposSequenceEditorSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

    //~ UDeveloperSettings Interface
    virtual FName GetContainerName() const override;
    virtual FName GetCategoryName() const override;
    //~

public:
    //TODO: add everything inside UMovieSceneToolsProjectSettings here ? or inherit from it ?
    // it is used inside:
    // ./Source/EposTracksEditor/Private/EposTracksEditorHelpers.cpp

    /** Specifies Board stuff. */
    UPROPERTY(config, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FBoardSettings BoardSettings;

    /** Specifies Shot stuff. */
    UPROPERTY(config, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FShotSettings ShotSettings;
};
