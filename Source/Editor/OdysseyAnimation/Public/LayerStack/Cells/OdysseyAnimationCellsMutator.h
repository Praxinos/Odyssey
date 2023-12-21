// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Mutations/OdysseyMutator.h"

class UObject;

class ODYSSEYANIMATION_API FOdysseyAnimationCellsMutator
    : public FOdysseyMutator
{
public:
    virtual ~FOdysseyAnimationCellsMutator() {};
    FOdysseyAnimationCellsMutator(UObject* iOwner, TSharedPtr<FOdysseyAnimationCellsContainer> iContainer);

public:
    virtual void Commit() override;
    virtual void Reset() override;

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

    TSharedPtr<FOdysseySetCellsOffsetMutation> mOffsetMutation;
    TMap<TSharedPtr<FOdysseyAnimationCell>, TSharedPtr<FOdysseySetCellLengthMutation>> mCellMutations;
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
    FOdysseySetCellLengthMutation(TSharedPtr<FOdysseyAnimationCell> iCell, int iNewLength, int iOldLength);

public:
    void Set(int iNewLength);

public:
    virtual bool IsDirty() const override;
    
    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    TSharedPtr<FOdysseyAnimationCell> mCell;
    int mNewLength; //Index of the cells
    int mOldLength; //Index of the cells
};

class ODYSSEYANIMATION_API FOdysseySetCellsOffsetMutation
    : public IOdysseyMutation
{
public:
    FOdysseySetCellsOffsetMutation(TSharedPtr<FOdysseyAnimationCellsContainer> iContainer, int iNewOffset, int iOldOffset);
    
public:
    void Set(int iNewOffset);

public:
    virtual bool IsDirty() const override;

    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    TSharedPtr<FOdysseyAnimationCellsContainer> mContainer;
    int mNewOffset; //Index of the cells
    int mOldOffset; //Index of the cells
};