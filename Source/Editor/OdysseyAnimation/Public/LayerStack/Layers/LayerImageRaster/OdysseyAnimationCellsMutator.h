// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Mutations/OdysseyMutator.h"

class UOdysseyAnimationLayerImageRaster;

class ODYSSEYANIMATION_API FOdysseyAnimationCellsMutator
    : public FOdysseyMutator
{
public:
    //FOdysseyAnimationCellsMutator(TSharedPtr<FOdysseyAnimationCellsContainer>& iCells,  bool iGenerateUndo = true);
    FOdysseyAnimationCellsMutator(UOdysseyAnimationLayerImageRaster* iLayer);

public:
    void Add(TArray<TSharedPtr<FOdysseyAnimationCell>> iCells, int iIndex = INDEX_NONE);
    void Remove(int iIndex, int iNumCells = 1);
    void SetLength(int iIndex, int iLength);
    void SetOffset(int iOffset);

private:
    UOdysseyAnimationLayerImageRaster* mLayer;

    //TWeakPtr<FOdysseyAnimationCellsContainer> mCellsContainer;
};

class ODYSSEYANIMATION_API FOdysseyAddCellsMutation
    : public IOdysseyMutation
{
public:
    FOdysseyAddCellsMutation(UOdysseyAnimationLayerImageRaster* iLayer, int iIndex, TArray<TSharedPtr<FOdysseyAnimationCell>> iCells);

public:
    static const FGuid& StaticID();

    //A unique identifier for the mutation type
    virtual const FGuid& GetID() override;

    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    UOdysseyAnimationLayerImageRaster* mLayer;
    int mIndex; //Index of the cells
    TArray<TSharedPtr<FOdysseyAnimationCell>> mCells;
};

class ODYSSEYANIMATION_API FOdysseyRemoveCellsMutation
    : public IOdysseyMutation
{
public:
    FOdysseyRemoveCellsMutation(UOdysseyAnimationLayerImageRaster* iLayer, int iIndex, TArray<TSharedPtr<FOdysseyAnimationCell>> iCells);

public:
    static const FGuid& StaticID();

    //A unique identifier for the mutation type
    virtual const FGuid& GetID() override;

    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    UOdysseyAnimationLayerImageRaster* mLayer;
    int mIndex; //Index of the cells
    TArray<TSharedPtr<FOdysseyAnimationCell>> mCells;
};

class ODYSSEYANIMATION_API FOdysseySetCellLengthMutation
    : public IOdysseyMutation
{
public:
    FOdysseySetCellLengthMutation(UOdysseyAnimationLayerImageRaster* iLayer, int iIndex, int iNewLength, int iOldLength);

public:
    static const FGuid& StaticID();

    //A unique identifier for the mutation type
    virtual const FGuid& GetID() override;

    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    UOdysseyAnimationLayerImageRaster* mLayer;
    int mIndex; //Index of the cells
    int mNewLength; //Index of the cells
    int mOldLength; //Index of the cells
};

class ODYSSEYANIMATION_API FOdysseySetCellsOffsetMutation
    : public IOdysseyMutation
{
public:
    FOdysseySetCellsOffsetMutation(UOdysseyAnimationLayerImageRaster* iLayer, int iNewOffset, int iOldOffset);
    
public:
    static const FGuid& StaticID();

    //A unique identifier for the mutation type
    virtual const FGuid& GetID() override;

    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    UOdysseyAnimationLayerImageRaster* mLayer;
    int mNewOffset; //Index of the cells
    int mOldOffset; //Index of the cells
};