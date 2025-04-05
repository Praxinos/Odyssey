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

public:
    friend bool operator==( const FAnimationCutEntry& A, const FAnimationCutEntry& B )
    {
        return A.mCellBefore == B.mCellBefore
            && A.mCellAfter == B.mCellAfter;
    }

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

    int32 Offset( int32 iOffset );

    void AddNewEntry( const FAnimationCutEntry& iAnimationCutEntry );

public:
    friend bool operator==( const FAnimationCut& A, const FAnimationCut& B )
    {
        return A.mAnimationCutEntries == B.mAnimationCutEntries;
    }

private:
    UPROPERTY()
    TArray<FAnimationCutEntry> mAnimationCutEntries;
};

USTRUCT()
struct ODYSSEYANIMATION_API FAnimationCuts
{
    GENERATED_BODY()

public:
    FAnimationCuts();

    FAnimationCuts( UOdysseyAnimation* iAnimation );

    void Build();

    const TMap<FFrameNumber, FAnimationCut>& GetMap() const;

    bool FindAnimationCutKey( const FAnimationCut& iAnimationCut, FFrameNumber& oFrame ) const;

    bool FindPreviousAnimationCut( const FAnimationCut& iAnimationCut, FAnimationCut& oPreviousAnimationCut ) const;
    bool FindNextAnimationCut( const FAnimationCut& iAnimationCut, FAnimationCut& oNextAnimationCut ) const;

    void UpdateAnimationCuts( const TArray<FAnimationCut>& iAnimationCuts, const TArray<FFrameNumber>& iNewFrames );

private:
    TObjectPtr<UOdysseyAnimation> mAnimation;

    TMap<FFrameNumber, FAnimationCut> mAnimationCutPerFrameMap;
};
