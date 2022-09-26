// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

//---

UCLASS( config=Epos )
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
