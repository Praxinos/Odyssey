// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#pragma once

#include "CoreMinimal.h"

#include "OdysseyAnimationCut.generated.h"

class UOdysseyAnimation;
class UOdysseyAnimationCell;

USTRUCT()
struct ODYSSEYANIMATION_API FAnimationCutEntry
{
    GENERATED_BODY()

public:
    FAnimationCutEntry();
    FAnimationCutEntry( UOdysseyAnimationCell* iCellBefore, UOdysseyAnimationCell* iCellAfter );

    FFrameNumber GetFrameReference() const;

    UOdysseyAnimationCell* GetCellReference();

    TRange<FFrameNumber> GetRangeLimit() const;

    void SetFrame( FFrameNumber iNewFrame );

private:
    UPROPERTY()
    TObjectPtr<UOdysseyAnimationCell> mCellBefore;
    UPROPERTY()
    TObjectPtr<UOdysseyAnimationCell> mCellAfter;
};

USTRUCT()
struct ODYSSEYANIMATION_API FAnimationCut
{
    GENERATED_BODY()

public:
    FAnimationCut();

    UOdysseyAnimation* GetAnimation();

    FFrameNumber GetFrameReference() const;
    TArray<UOdysseyAnimationCell*> GetCellsReference() const;
    TRange<FFrameNumber> GetRangeLimit() const;

    void Offset( int32 iOffset );

    void AddNewEntry( const FAnimationCutEntry& iAnimationCutEntry );

private:
    UPROPERTY()
    TArray<FAnimationCutEntry> mAnimationCutEntries;
};
