// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "CineCameraComponent.h"
#include "Engine/DeveloperSettings.h"
#include "Misc/FrameRate.h"
#include "UObject/Object.h"

#include "PatternKeywordList.h"

#include "EposSequenceEditorSettings.generated.h"

//---

USTRUCT( BlueprintType )
struct FBoardSettings
{
    GENERATED_BODY()

public:
    /** Default tick frame rate of new Board Sequences. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Board)
    FFrameRate DefaultTickFrameRate { 24000, 1 };

    /** Default display frame rate of new Board Sequences. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Board)
    FFrameRate DefaultDisplayFrameRate { 24, 1 };

};

//---

USTRUCT( BlueprintType )
struct FShotSettings
{
    GENERATED_BODY()

public:
    /** Default tick frame rate of new Shot Sequences. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Shot)
    FFrameRate DefaultTickFrameRate { 24000, 1 };

    /** Default display frame rate of new Shot Sequences. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Shot)
    FFrameRate DefaultDisplayFrameRate { 24, 1 };
};

//---

USTRUCT( BlueprintType )
struct FNoteSettings
{
    GENERATED_BODY()

public:
    /**
     * Display notes in viewport.
     *
     * Display the Notes between the camera view and the playback
     * controls of the Storyboard Viewport.
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Note)
    bool DisplayNoteInViewport { false };

    /**
     * Display notes as overlay.
     *
     * Display the Notes as subtitles in the camera view.
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Note)
    bool DisplayNoteAsOverlay { false };
};

//---

USTRUCT( BlueprintType )
struct FStoryboardViewportSettings
{
    GENERATED_BODY()

public:
    /**
     * Display playback track.
     *
     * Display a tiny track that shows what is before the current frame
     * (in white) and what is after it (in grey).
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Storyboard Viewport")
    bool DisplayPlaybackTrack { true };

    /**
     * Display playback controls.
     *
     * Display the remote control to play the sequence.
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Storyboard Viewport")
    bool DisplayPlaybackControls { true };

    /**
     * Display sequence info.
     *
     * Display the name of the Sequence on the left and the current
     * frame on the right.
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Storyboard Viewport")
    bool DisplaySequenceInfos { true };

    /**
     * Display actor controls.
     *
     * Display the Planes and Camera controls (Plane Distance and
     * Camera Focal Length).
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Storyboard Viewport")
    bool DisplayActorControls { true };

    /**
     * Display camera bounds.
     *
     * Apply a watermark color on the viewport beyond the camera view.
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Storyboard Viewport")
    bool bDisplayCameraBounds { true };

    /** Color and opacity of the watermark applied beyond the camera view. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Storyboard Viewport")
    FLinearColor CameraBoundsShadeColor = FLinearColor(0.f, 0.f, 0.f, 0.8f);

    /**
     * Display camera piloting indication.
     *
     * Show a visual indication in the viewport while a camera is
     * being piloted.
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Storyboard Viewport")
    bool bDisplayPilotingCameraHUD { true };
};

//---

enum class EInfoBarPatternKeyword : uint32
{
    ENUM_UNIQUE_ID( CurrentFrame_InStoryboard ),
    ENUM_UNIQUE_ID( CurrentFrame_InSequence ),
    ENUM_UNIQUE_ID( CurrentFrame_InSubsequence ),

    ENUM_UNIQUE_ID( StartFrameOfStoryboard_InStoryboard ),
    ENUM_UNIQUE_ID( StopFrameOfStoryboard_InStoryboard ),

    ENUM_UNIQUE_ID( StartFrameOfSequence_InStoryboard ),
    ENUM_UNIQUE_ID( StartFrameOfSequence_InSequence ),

    ENUM_UNIQUE_ID( StopFrameOfSequence_InStoryboard ),
    ENUM_UNIQUE_ID( StopFrameOfSequence_InSequence ),

    ENUM_UNIQUE_ID( StartFrameOfSubsequence_InStoryboard ),
    ENUM_UNIQUE_ID( StartFrameOfSubsequence_InSequence ),
    ENUM_UNIQUE_ID( StartFrameOfSubsequence_InSubSequence ),

    ENUM_UNIQUE_ID( StopFrameOfSubsequence_InStoryboard ),
    ENUM_UNIQUE_ID( StopFrameOfSubsequence_InSequence ),
    ENUM_UNIQUE_ID( StopFrameOfSubsequence_InSubSequence ),

    //---

    ENUM_UNIQUE_ID( Storyboard_Duration ),
    ENUM_UNIQUE_ID( Storyboard_TotalSequences ), // Total: recursive

    ENUM_UNIQUE_ID( Sequence_Duration ),
    ENUM_UNIQUE_ID( Sequence_NumberOfSubsequences ), // NumberOf: in the same level
    ENUM_UNIQUE_ID( Sequence_Index ),
    ENUM_UNIQUE_ID( Sequence_Name ),

    ENUM_UNIQUE_ID( Subsequence_Duration ),
    ENUM_UNIQUE_ID( Subsequence_Index ),
    ENUM_UNIQUE_ID( Subsequence_Name ),
};

const FPatternKeywordList& GetInfoBarPatternKeywordList();

USTRUCT( BlueprintType )
struct FInfoBarSettings
{
    GENERATED_BODY()

public:
    FInfoBarSettings();

public:
    /**
     * Pattern of the name displayed at the top of the Sequencer.
     *
     * The displayed information depends on the cursor position.
     * Each line of the pattern is joined with the separator.
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=InfoBar, meta=(MultiLine="true"))
    FString Pattern;

    FPatternKeywordLists mPatternKeywordLists;

    /** Separator inserted between the lines of the info bar pattern. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=InfoBar)
    FString Separator { TEXT( " - " ) };
};

//---

/**
 * Epos Sequence Editor settings.
 */
UCLASS(BlueprintType, config=Odyssey, meta=(DisplayName="Odyssey - Storyboard Sequence Editor"))
class UEposSequenceEditorSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

    //~ UDeveloperSettings Interface
    virtual FName GetContainerName() const override;
    virtual FName GetCategoryName() const override;
    //~

public:
    /** Specifies Board stuff. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ShowOnlyInnerProperties))
    FBoardSettings BoardSettings;

    /** Specifies Shot stuff. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ShowOnlyInnerProperties))
    FShotSettings ShotSettings;

    /** Specifies Note stuff. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ShowOnlyInnerProperties))
    FNoteSettings NoteSettings;

    /** Specifies InfoBar stuff. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ShowOnlyInnerProperties))
    FInfoBarSettings InfoBarSettings;

    /** Specifies Viewport stuff. */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ShowOnlyInnerProperties))
    FStoryboardViewportSettings ViewportSettings;
};
