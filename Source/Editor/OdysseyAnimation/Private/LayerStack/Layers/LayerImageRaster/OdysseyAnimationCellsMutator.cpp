// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationCellsMutator.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"


FOdysseyAnimationCellsMutator::FOdysseyAnimationCellsMutator(UObject* iOwner, TSharedRef<FOdysseyAnimationCellsContainer> iContainer)
    : FOdysseyMutator(iOwner, "FOdysseyAnimationCellsMutator")
    , mContainer(iContainer)
{
    GetRootMutation()->OnCommited().BindLambda(
        [this]()
        {
            mContainer->OnCellsChanged().Broadcast();
        }
    );
}

void
FOdysseyAnimationCellsMutator::Add(TArray<TSharedPtr<FOdysseyAnimationCell>> iCells, int iIndex)
{
    int index = iIndex < 0 ? mContainer->mCells.Num() : iIndex;
    TSharedPtr<FOdysseyAddCellsMutation> mutation = MakeShared<FOdysseyAddCellsMutation>(mContainer, index, iCells);
    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationCellsMutator::Remove(int iIndex, int iNumCells)
{
    if (iNumCells < 1 || iIndex < 0 || iIndex + iNumCells > mContainer->mCells.Num())
        return;
        
    TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
    for (int i = 0; i < iNumCells; i++)
    {
        TSharedPtr<FOdysseyAnimationCell> cell = mContainer->mCells[i + iIndex];
        cells.Add(cell);
    }

    TSharedPtr<FOdysseyRemoveCellsMutation> mutation = MakeShared<FOdysseyRemoveCellsMutation>(mContainer, iIndex, cells);
    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationCellsMutator::SetLength(int iIndex, int iLength)
{
    if (iIndex < 0 || iIndex >= mContainer->mCells.Num())
        return;

    int oldLength = mContainer->mCells[iIndex]->GetLength();

    TSharedPtr<FOdysseySetCellLengthMutation> mutation = MakeShared<FOdysseySetCellLengthMutation>(mContainer, iIndex, iLength, oldLength);
    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationCellsMutator::SetOffset(int iOffset)
{
    TSharedPtr<FOdysseySetCellsOffsetMutation> mutation = MakeShared<FOdysseySetCellsOffsetMutation>(mContainer, iOffset, mContainer->GetOffset());
    AddAndApplyMutation(mutation);
}

//=======================================================================================

FOdysseyAddCellsMutation::FOdysseyAddCellsMutation(TSharedRef<FOdysseyAnimationCellsContainer> iContainer, int iIndex, TArray<TSharedPtr<FOdysseyAnimationCell>> iCells)
    : mContainer(iContainer)
    , mIndex(iIndex)
    , mCells(iCells)
{
}

void
FOdysseyAddCellsMutation::Apply()
{
    mContainer->mCells.Insert(mCells, mIndex);
}

void
FOdysseyAddCellsMutation::Revert()
{
    mContainer->mCells.RemoveAt(mIndex, mCells.Num());
}

//=======================================================================================

FOdysseyRemoveCellsMutation::FOdysseyRemoveCellsMutation(TSharedRef<FOdysseyAnimationCellsContainer> iContainer, int iIndex, TArray<TSharedPtr<FOdysseyAnimationCell>> iCells)
    : mContainer(iContainer)
    , mIndex(iIndex)
    , mCells(iCells)
{
}

void
FOdysseyRemoveCellsMutation::Apply()
{
    mContainer->mCells.RemoveAt(mIndex, mCells.Num());
}

void
FOdysseyRemoveCellsMutation::Revert()
{
    mContainer->mCells.Insert(mCells, mIndex);
}

//=======================================================================================

FOdysseySetCellLengthMutation::FOdysseySetCellLengthMutation(TSharedRef<FOdysseyAnimationCellsContainer> iContainer, int iIndex, int iNewLength, int iOldLength)
    : mContainer(iContainer)
    , mIndex(iIndex)
    , mNewLength(iNewLength)
    , mOldLength(iOldLength)
{
}

void
FOdysseySetCellLengthMutation::Apply()
{
    mContainer->mCells[mIndex]->SetLength(mNewLength);
}

void
FOdysseySetCellLengthMutation::Revert()
{
    mContainer->mCells[mIndex]->SetLength(mOldLength);
}

//=======================================================================================


FOdysseySetCellsOffsetMutation::FOdysseySetCellsOffsetMutation(TSharedRef<FOdysseyAnimationCellsContainer> iContainer, int iNewOffset, int iOldOffset)
    : mContainer(iContainer)
    , mNewOffset(iNewOffset)
    , mOldOffset(iOldOffset)
{
}

void
FOdysseySetCellsOffsetMutation::Apply()
{
    mContainer->mOffset = mNewOffset;
}

void
FOdysseySetCellsOffsetMutation::Revert()
{
    mContainer->mOffset = mOldOffset;
}
