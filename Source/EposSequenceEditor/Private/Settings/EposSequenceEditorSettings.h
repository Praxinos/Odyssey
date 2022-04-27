// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
};

//---

USTRUCT()
struct FNoteSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(config, EditAnywhere, Category=Note)
    bool DisplayNoteInViewport { false };

    UPROPERTY(config, EditAnywhere, Category=Note)
    bool DisplayNoteAsOverlay { false };
};

//---

UENUM()
enum class EInfoBarPatternKeyword : uint8
{
    CurrentFrame_InStoryboard,
    CurrentFrame_InSequence,
    CurrentFrame_InSubsequence,

    StartFrameOfStoryboard_InStoryboard,
    StopFrameOfStoryboard_InStoryboard,

    StartFrameOfSequence_InStoryboard,
    StartFrameOfSequence_InSequence,

    StopFrameOfSequence_InStoryboard,
    StopFrameOfSequence_InSequence,

    StartFrameOfSubsequence_InStoryboard,
    StartFrameOfSubsequence_InSequence,
    StartFrameOfSubsequence_InSubSequence,

    StopFrameOfSubsequence_InStoryboard,
    StopFrameOfSubsequence_InSequence,
    StopFrameOfSubsequence_InSubSequence,

    //---

    Storyboard_Duration,
    Storyboard_TotalSequences, // Total: recursive

    Sequence_Duration,
    Sequence_NumberOfSubsequences, // NumberOf: in the same level
    Sequence_Index,
    Sequence_Name,

    Subsequence_Duration,
    Subsequence_Index,
    Subsequence_Name,
};

USTRUCT()
struct FInfoBarPatternKeyword
{
    GENERATED_BODY()

public:
    EInfoBarPatternKeyword mKeywordId;
    FString mKeywordWithBraces;
    FText mHelp;
};

USTRUCT()
struct FInfoBarSettings
{
    GENERATED_BODY()

public:
    FInfoBarSettings();

public:
    UPROPERTY(config, EditAnywhere, Category=InfoBar, meta=(MultiLine="true"))
    FString Pattern;

    /** List of all keywords.
        This list is hidden in customization.
        If UPROPERTY is empty, there is no access through IPropertyHandle in customization
    */
    UPROPERTY(VisibleAnywhere, Category=InfoBar, Transient)
    TMap<EInfoBarPatternKeyword, FInfoBarPatternKeyword> PatternKeywords;

    UPROPERTY(config, EditAnywhere, Category=InfoBar)
    FString Separator { TEXT( " - " ) };
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
    /** Specifies Board stuff. */
    UPROPERTY(config, EditAnywhere, Category=Settings, meta=(ShowOnlyInnerProperties))
    FBoardSettings BoardSettings;

    /** Specifies Shot stuff. */
    UPROPERTY(config, EditAnywhere, Category=Settings, meta=(ShowOnlyInnerProperties))
    FShotSettings ShotSettings;

    /** Specifies Note stuff. */
    UPROPERTY(config, EditAnywhere, Category=Settings, meta=(ShowOnlyInnerProperties))
    FNoteSettings NoteSettings;

    /** Specifies InfoBar stuff. */
    UPROPERTY(config, EditAnywhere, Category=Settings, meta=(ShowOnlyInnerProperties))
    FInfoBarSettings InfoBarSettings;
};
