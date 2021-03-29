// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "CineCameraComponent.h"
#include "UObject/Object.h"

#include "EposSequenceEditorSettings.generated.h"

//---

USTRUCT()
struct FBoardSettings
{
    GENERATED_BODY()

public:
    FBoardSettings();

    UPROPERTY(config, EditAnywhere, Category=Board)
    FFrameRate DefaultTickFrameRate;

    UPROPERTY(config, EditAnywhere, Category=Board)
    FFrameRate DefaultDisplayFrameRate;
};

//---

USTRUCT()
struct FShotSettings
{
    GENERATED_BODY()

public:
    FShotSettings();

    UPROPERTY(config, EditAnywhere, Category=Shot)
    FFrameRate DefaultTickFrameRate;

    UPROPERTY(config, EditAnywhere, Category=Shot)
    FFrameRate DefaultDisplayFrameRate;
};

//---

USTRUCT()
struct FCameraSettings
{
    GENERATED_BODY()

public:
    /** Controls the filmback of the camera. */
    UPROPERTY( config, EditAnywhere, Category = Camera )
    FCameraFilmbackSettings Filmback;

    /** Controls the camera's lens. */
    UPROPERTY( config, EditAnywhere, Category = Camera )
    FCameraLensSettings LensSettings;

    /** Current focal length of the camera (i.e. controls FoV, zoom) */
    UPROPERTY( config, EditAnywhere, Category = Camera )
    float CurrentFocalLength;

    /** Current aperture, in terms of f-stop (e.g. 2.8 for f/2.8) */
    UPROPERTY( config, EditAnywhere, Category = Camera )
    float CurrentAperture;

#if WITH_EDITORONLY_DATA
    /** Read-only. Control this value with CurrentFocalLength (and filmback settings). */
    UPROPERTY( config, VisibleAnywhere, Category = Camera )
    float CurrentHorizontalFOV;
#endif
};

//---

/**
 * Epos Sequence Editor settings.
 */
UCLASS(config=Epos)
class UEposSequenceEditorSettings
    : public UObject
{
    GENERATED_BODY()

protected:
    virtual void PostInitProperties() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif

private:
    void UpdateValues();

public:
    //TODO: add everything inside UMovieSceneToolsProjectSettings here ? or inherit from it ?
    // it is used inside:
    // ./Source/EposSequenceEditor/Private/Board/BoardSequenceFactoryNew.cpp
    // ./Source/EposSequenceEditor/Private/Shot/ShotSequenceFactoryNew.cpp
    // ./Source/EposTracksEditor/Private/EposTracksEditorHelpers.cpp

    /** Specifies Board stuff. */
    UPROPERTY(config, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FBoardSettings BoardSettings;

    /** Specifies Shot stuff. */
    UPROPERTY(config, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FShotSettings ShotSettings;

    UPROPERTY(config, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FCameraSettings CameraSettings;
};
