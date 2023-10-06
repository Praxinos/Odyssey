// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Mutations/OdysseyMutator.h"

class UObject;

class ODYSSEYANIMATION_API FOdysseyAnimationCellsMutator
    : public FOdysseyMutator
{
public:
    FOdysseyAnimationCellsMutator(UObject* iOwner, TSharedPtr<FOdysseyAnimationCellsContainer> iContainer);

public:
    void Add(TArray<TSharedPtr<FOdysseyAnimationCell>> iCells, int iIndex = INDEX_NONE);
    void AddAtFrame(TArray<TSharedPtr<FOdysseyAnimationCell>> iCells, int iFrame);
    void BreakCellAtFrame(int iFrame);
    void Remove(int iIndex, int iNumCells = 1);
    void RemoveFrame(int iFrameIndex);
    void RemoveFrameRange(const FInt32Range& iRange);
    void SetLength(int iIndex, int iLength);
    void SetOffset(int iOffset);

private:
    TSharedPtr<FOdysseyAnimationCellsContainer> mContainer;
};

class ODYSSEYANIMATION_API FOdysseyAddCellsMutation
    : public IOdysseyMutation
{
public:
    FOdysseyAddCellsMutation(TSharedPtr<FOdysseyAnimationCellsContainer> iContainer, int iIndex, TArray<TSharedPtr<FOdysseyAnimationCell>> iCells);

public:
    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    TSharedPtr<FOdysseyAnimationCellsContainer> mContainer;
    int mIndex; //Index of the cells
    TArray<TSharedPtr<FOdysseyAnimationCell>> mCells;
};

class ODYSSEYANIMATION_API FOdysseyRemoveCellsMutation
    : public IOdysseyMutation
{
public:
    FOdysseyRemoveCellsMutation(TSharedPtr<FOdysseyAnimationCellsContainer> iContainer, int iIndex, TArray<TSharedPtr<FOdysseyAnimationCell>> iCells);

public:
    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    TSharedPtr<FOdysseyAnimationCellsContainer> mContainer;
    int mIndex; //Index of the cells
    TArray<TSharedPtr<FOdysseyAnimationCell>> mCells;
};

class ODYSSEYANIMATION_API FOdysseySetCellLengthMutation
    : public IOdysseyMutation
{
public:
    FOdysseySetCellLengthMutation(TSharedPtr<FOdysseyAnimationCellsContainer> iContainer, int iIndex, int iNewLength, int iOldLength);

public:
    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    TSharedPtr<FOdysseyAnimationCellsContainer> mContainer;
    int mIndex; //Index of the cells
    int mNewLength; //Index of the cells
    int mOldLength; //Index of the cells
};

class ODYSSEYANIMATION_API FOdysseySetCellsOffsetMutation
    : public IOdysseyMutation
{
public:
    FOdysseySetCellsOffsetMutation(TSharedPtr<FOdysseyAnimationCellsContainer> iContainer, int iNewOffset, int iOldOffset);
    
public:
    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    TSharedPtr<FOdysseyAnimationCellsContainer> mContainer;
    int mNewOffset; //Index of the cells
    int mOldOffset; //Index of the cells
};