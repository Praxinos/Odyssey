// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "CineCameraComponent.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/Object.h"

#include "PatternKeywordList.h"

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

USTRUCT()
struct FInfoBarSettings
{
    GENERATED_BODY()

public:
    FInfoBarSettings();

public:
    UPROPERTY(config, EditAnywhere, Category=InfoBar, meta=(MultiLine="true"))
    FString Pattern;

    FPatternKeywordLists mPatternKeywordLists;

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
