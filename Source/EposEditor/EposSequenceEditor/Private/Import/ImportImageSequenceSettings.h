// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "PatternKeywordList.h"

#include "ImportImageSequenceSettings.generated.h"

//---

enum class EImportImageSequencePatternKeyword : uint32
{
    ENUM_UNIQUE_ID( BoardId ),
    ENUM_UNIQUE_ID( ShotId ),
    ENUM_UNIQUE_ID( PanelId ),
    ENUM_UNIQUE_ID( Duration ),
};

const FPatternKeywordList& GetImportImageSequencePatternKeywordList();

//---

USTRUCT( BlueprintType )
struct FImportImageSequenceOptions
{
    GENERATED_BODY()

public:
    FImportImageSequenceOptions();

public:
    /** Image sequence folder. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ImportImageSequence )
    FDirectoryPath ImageSequencePath;

    /** Image name pattern. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ImportImageSequence )
    FString FilePattern;

    FPatternKeywordLists mPatternKeywordLists;

    /** Override the duration of all panels. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ImportImageSequenceOptions, meta=(InlineEditConditionToggle) )
    bool bShouldOverrideDuration = false;

    /** The duration to override all panels. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category=ImportImageSequenceOptions, meta=(EditCondition=bShouldOverrideDuration) )
    int32 OverrideDuration = 64;
};

//---

// https://udn.unrealengine.com/s/question/0D54z00007eBzPfCAK/makeinstance-of-ipropertytypecustomization-of-my-structure-is-not-called
UCLASS( config=Odyssey )
class UImportImageSequenceSettings
    : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY( config, EditAnywhere, BlueprintReadWrite, Category=ImportImageSequence )
    FImportImageSequenceOptions Options;
};

//---

UCLASS( config=Odyssey )
class UImportImageSequenceUISettings
    : public UObject
{
    GENERATED_BODY()

public:
    float GetThumbnailScaleMultiplier() const;
    void SetThumbnailScaleMultiplier( float iMultiplier );

private:
    /** Storyboard path. */
    UPROPERTY( config, EditAnywhere, Category=ImportImageSequence )
    float mThumbnailScaleMultiplier { 100.f /* % */ };
};
