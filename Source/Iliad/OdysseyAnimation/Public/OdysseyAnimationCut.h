// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyAnimationCut.generated.h"

class UOdysseyAnimation;
class UOdysseyLayerCell;

USTRUCT()
struct ODYSSEYANIMATION_API FAnimationCutEntry
{
    GENERATED_BODY()

public:
    FAnimationCutEntry();
    FAnimationCutEntry( UOdysseyLayerCell* iCellBefore, UOdysseyLayerCell* iCellAfter );

    FFrameNumber GetFrameReference() const;

    UOdysseyLayerCell* GetCellReference();

    TRange<FFrameNumber> GetRangeLimit() const;

    void SetFrame( FFrameNumber iNewFrame, EPropertyChangeType::Type iChangeType );

private:
    UPROPERTY()
    TObjectPtr<UOdysseyLayerCell> mCellBefore;
    UPROPERTY()
    TObjectPtr<UOdysseyLayerCell> mCellAfter;
};

USTRUCT()
struct ODYSSEYANIMATION_API FAnimationCut
{
    GENERATED_BODY()

public:
    FAnimationCut();

    UOdysseyAnimation* GetAnimation();

    FFrameNumber GetFrameReference() const;
    TArray<UOdysseyLayerCell*> GetCellsReference() const;
    TRange<FFrameNumber> GetRangeLimit() const;

    void Offset( int32 iOffset, EPropertyChangeType::Type iChangeType );
    int32 AdjustOffset( int32 iOffset ) const;

    void AddNewEntry( const FAnimationCutEntry& iAnimationCutEntry );

private:
    UPROPERTY()
    TArray<FAnimationCutEntry> mAnimationCutEntries;
};
