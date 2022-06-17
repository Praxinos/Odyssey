// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "ImportImageSequenceSettings.generated.h"

//---

UENUM()
enum class EImportImageSequencePatternKeyword : uint8
{
    BoardId,
    ShotId,
    FrameId,
    Duration,
};

USTRUCT()
struct FImportImageSequencePatternKeyword
{
    GENERATED_BODY()

public:
    EImportImageSequencePatternKeyword mKeywordId;
    FString mKeywordWithBraces;
    FText mHelp;
};

USTRUCT()
struct FImportImageSequenceOptions
{
    GENERATED_BODY()

public:
    FImportImageSequenceOptions();

public:
    /** Image sequence folder. */
    UPROPERTY( EditAnywhere, Category=ImportImageSequence )
    FDirectoryPath ImageSequencePath;

    /** Image name pattern. */
    UPROPERTY( EditAnywhere, Category=ImportImageSequence )
    FString FilePattern;

    /** List of all keywords.
        This list is hidden in customization.
        If UPROPERTY is empty, there is no access through IPropertyHandle in customization
    */
    UPROPERTY( VisibleAnywhere, Category=ImportImageSequence, Transient )
    TMap<EImportImageSequencePatternKeyword, FImportImageSequencePatternKeyword> PatternKeywords;
};

//---

// https://udn.unrealengine.com/s/question/0D54z00007eBzPfCAK/makeinstance-of-ipropertytypecustomization-of-my-structure-is-not-called
UCLASS( config=Epos )
class UImportImageSequenceSettings
    : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY( config, EditAnywhere, Category=ImportImageSequence )
    FImportImageSequenceOptions Options;
};
