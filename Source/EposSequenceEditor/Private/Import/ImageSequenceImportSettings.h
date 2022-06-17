// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "ImageSequenceImportSettings.generated.h"

//---

UENUM()
enum class EImageSequencePatternKeyword : uint8
{
    BoardId,
    ShotId,
    FrameId,
    Duration,
};

USTRUCT()
struct FImageSequencePatternKeyword
{
    GENERATED_BODY()

public:
    EImageSequencePatternKeyword mKeywordId;
    FString mKeywordWithBraces;
    FText mHelp;
};

USTRUCT()
struct FImageSequenceImportOptions
{
    GENERATED_BODY()

public:
    FImageSequenceImportOptions();

public:
    /** Image sequence folder. */
    UPROPERTY( EditAnywhere, Category=ImageSequenceImport )
    FDirectoryPath ImageSequencePath;

    /** Image name pattern. */
    UPROPERTY( EditAnywhere, Category=ImageSequenceImport )
    FString FilePattern;

    /** List of all keywords.
        This list is hidden in customization.
        If UPROPERTY is empty, there is no access through IPropertyHandle in customization
    */
    UPROPERTY( VisibleAnywhere, Category=ImageSequenceImport, Transient )
    TMap<EImageSequencePatternKeyword, FImageSequencePatternKeyword> PatternKeywords;
};

//---

// https://udn.unrealengine.com/s/question/0D54z00007eBzPfCAK/makeinstance-of-ipropertytypecustomization-of-my-structure-is-not-called
UCLASS( config=Epos )
class UImageSequenceImportSettings
    : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY( config, EditAnywhere, Category=ImageSequenceImport )
    FImageSequenceImportOptions Options;
};
