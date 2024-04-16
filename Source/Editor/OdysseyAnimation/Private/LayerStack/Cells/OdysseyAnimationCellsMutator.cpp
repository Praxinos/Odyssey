// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCellsMutator.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"


FOdysseyAnimationCellsMutator::FOdysseyAnimationCellsMutator(UObject* iOwner, TSharedPtr<FOdysseyAnimationCellsContainer> iContainer)
    : FOdysseyMutator(iOwner, "FOdysseyAnimationCellsMutator")
    , mContainer(iContainer)
    , mOffsetMutation(nullptr)
{
    GetRootMutation()->OnCommited().BindLambda(
        [container = mContainer]()
        {
            container->OnCellsChanged().Broadcast();
        }
    );
}

void
FOdysseyAnimationCellsMutator::Commit()
{
    //Sanitize cells (remove all cells with length <= 0)
    TArray<TSharedPtr<FOdysseyAnimationCell>> cells = mContainer->GetCells();
    for (int i = cells.Num() - 1; i >= 0 ; i--)
    {
        if (cells[i]->GetLength() <= 0)
            Remove(i, 1);
    }

    FOdysseyMutator::Commit();
}

void
FOdysseyAnimationCellsMutator::Reset()
{
    mOffsetMutation = nullptr;
    mCellMutations.Empty();
    FOdysseyMutator::Reset();

    GetRootMutation()->OnCommited().BindLambda(
        [container = mContainer]()
        {
            container->OnCellsChanged().Broadcast();
        }
    );
}

void
FOdysseyAnimationCellsMutator::Add(TArray<TSharedPtr<FOdysseyAnimationCell>> iCells, int iIndex)
{
    for(TSharedPtr<FOdysseyAnimationCell> cell : iCells)
    {
        checkf(mContainer->SupportsType(cell->GetType()), TEXT("cell type not supported"));
    }

    int index = iIndex < 0 ? mContainer->mCells.Num() : iIndex;
    TSharedPtr<FOdysseyAddCellsMutation> mutation = MakeShared<FOdysseyAddCellsMutation>(mContainer, index, iCells);
    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationCellsMutator::AddAtFrame(TArray<TSharedPtr<FOdysseyAnimationCell>> iCells, int iFrame)
{
    for(TSharedPtr<FOdysseyAnimationCell> cell : iCells)
    {
        checkf(mContainer->SupportsType(cell->GetType()), TEXT("cell type not supported"));
    }

    checkf(iFrame != INDEX_NONE, TEXT("Needs a valid frame Index"));
    if (iFrame == INDEX_NONE)
        return;

    BreakCellAtFrame(iFrame); //break cell if needed

    /**
     * @brief Get the Range of frames covered by cells
     */
    FInt32Range frameRange = mContainer->GetFrameRange();

    if (iFrame < frameRange.GetLowerBoundValue()) //out of container bounds
    {
        //The behaviour we chose is to :
        // - offset the cells to the right if needed to insert all the cells
        // - extend the first cell to join to the inserted cells if needed

        TSharedPtr<FOdysseyAnimationCell> firstCell = mContainer->mCells.Num() > 0 ? mContainer->mCells[0] : nullptr;
        if (firstCell)
        {
            int cellsLength = 0;
            for (TSharedPtr<FOdysseyAnimationCell> cell : iCells)
                cellsLength += cell->GetLength();

            int firstCellExtension = (mContainer->GetOffset() - iFrame) - cellsLength;
            if (firstCellExtension > 0)
                SetLength(0, firstCell->GetLength() + firstCellExtension);
        }
        
        SetOffset(iFrame); //Offset will always be at iFrame
        Add(iCells, 0); //Insert all the cells the beginning of the container
        return;
    }

    if (iFrame > frameRange.GetUpperBoundValue() + 1) //out of container bounds and needs frame extension
    {
        TSharedPtr<FOdysseyAnimationCell> lastCell = mContainer->mCells.Num() > 0 ? mContainer->mCells.Last() : nullptr;
        if (lastCell)
        {
            int lastCellExtension = iFrame - frameRange.GetUpperBoundValue() - 1;
            SetLength(mContainer->mCells.Num() - 1, lastCell->GetLength() +  lastCellExtension);
            Add(iCells); //Insert all the cells the end of the container
            return;
        }
        else
        {
            SetOffset(iFrame); //Offset will always be at iFrame
            Add(iCells, 0); //Insert all the cells the beginning of the container*
            return;
        }
    }

    int cellIndex = mContainer->GetCellIndexAtFrame(iFrame);
    Add(iCells, cellIndex);
}

void
FOdysseyAnimationCellsMutator::BreakCellAtFrame(int iFrame)
{
    FInt32Range frameRange = mContainer->GetFrameRange();

    if (iFrame < frameRange.GetLowerBoundValue() || iFrame > frameRange.GetUpperBoundValue()) //out of container bounds
        return;

    int cellFrame = mContainer->GetCellFrameAtFrame(iFrame);
    if (cellFrame == INDEX_NONE || cellFrame == 0)
        return;

    int cellIndex = mContainer->GetCellIndexAtFrame(iFrame);
    if (cellIndex == INDEX_NONE)
        return;

    TSharedPtr<FOdysseyAnimationCell> cell = mContainer->mCells[cellIndex];
    if (!cell)
        return;

    TSharedPtr<FOdysseyAnimationCell> newCell = cell->CreateCellFromFrame(cellFrame);
    if (!newCell)
        return;

    int newCellLength = cell->GetLength() - cellFrame;
    SetLength(cellIndex, cellFrame);
    Add({newCell}, cellIndex + 1);
    SetLength(cellIndex + 1, newCellLength);
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
FOdysseyAnimationCellsMutator::Remove(const TArray<TSharedPtr<FOdysseyAnimationCell>>& iCells, bool iPreventEmptyLayer)
{
    struct FCellPosition
    {
        int mIndex;
        TSharedPtr<FOdysseyAnimationCell> mCell;
    };

    TArray<FCellPosition> cellPositions;
    for (TSharedPtr<FOdysseyAnimationCell> cell : iCells)
    {
        int index = mContainer->mCells.Find(cell);
        if (index == INDEX_NONE)
            continue;

        cellPositions.Add({index, cell});
    }

    cellPositions.Sort(
        [this](const FCellPosition& iCellA, const FCellPosition& iCellB)
        {
            return iCellA.mIndex > iCellB.mIndex; //In Reverse order
        }
    );

    if (cellPositions.IsEmpty())
        return;

    if (iPreventEmptyLayer && cellPositions.Num() == mContainer->mCells.Num())
        cellPositions.RemoveAt(cellPositions.Num() - 1); //reverse order : we actually remove the first cell from cellPositions here

    int lastCellIndex = -1;
    TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
    for (const FCellPosition& cellPosition : cellPositions)
    {
        if (cellPosition.mIndex == lastCellIndex - 1) //remember, we sorted the array in reverse order, so we use -1
        {
            lastCellIndex = cellPosition.mIndex;
            cells.Add(cellPosition.mCell);
            continue;
        }

        if (!cells.IsEmpty())
        {
            TSharedPtr<FOdysseyRemoveCellsMutation> mutation = MakeShared<FOdysseyRemoveCellsMutation>(mContainer, lastCellIndex, cells);
            AddAndApplyMutation(mutation);
        }

        lastCellIndex = cellPosition.mIndex;
        cells.Empty();
        cells.Add(cellPosition.mCell);
    }

    //manage last cells
    TSharedPtr<FOdysseyRemoveCellsMutation> mutation = MakeShared<FOdysseyRemoveCellsMutation>(mContainer, lastCellIndex, cells);
    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationCellsMutator::RemoveFrame(int iFrameIndex)
{
    int cellIndex = mContainer->GetCellIndexAtFrame(iFrameIndex);
    if (cellIndex == INDEX_NONE)
        return;
    
    int cellLength = mContainer->GetCells()[cellIndex]->GetLength();
    if (cellLength == 1)
    {
        Remove(cellIndex);
    }
    else
    {
        SetLength(cellIndex, cellLength - 1);
    }
}

void
FOdysseyAnimationCellsMutator::RemoveFrameRange(const FInt32Range& iRange)
{
    bool isLowerClosed = iRange.GetLowerBound().IsClosed();
    bool isUpperClosed = iRange.GetUpperBound().IsClosed();

    if ( !isLowerClosed || !isUpperClosed )
        return;

    int startFrame = iRange.GetLowerBoundValue();
    int endFrame = iRange.GetUpperBoundValue();

    //Travel Backward to keep frame indexes in sync with cells modifications/removal
    //Should the last cell in frame range shrinked or removed ?
    int lastCellIndex = mContainer->GetCellIndexAtFrame(endFrame);
    int firstCellIndex = mContainer->GetCellIndexAtFrame(startFrame);

    if (lastCellIndex == firstCellIndex && lastCellIndex != INDEX_NONE && firstCellIndex != INDEX_NONE)
    {
        int cellLength = mContainer->GetCells()[lastCellIndex]->GetLength();
        int lengthToRemove = endFrame - startFrame + 1;
        if (lengthToRemove >= cellLength)
            Remove(lastCellIndex);
        else
            SetLength(lastCellIndex, cellLength - lengthToRemove);
        return;
    }

    if (lastCellIndex != INDEX_NONE)
    {
        int cellFrame = mContainer->GetCellFrameAtFrame(endFrame);
        int cellLength = mContainer->GetCells()[lastCellIndex]->GetLength();
        if (cellFrame == cellLength - 1)
        {
            Remove(lastCellIndex);
        }
        else
        {
            SetLength(lastCellIndex, cellLength - (cellFrame + 1));
        }
    }

    //Remove cells
    int firstCellIndexToRemove = firstCellIndex == INDEX_NONE ? 0 : firstCellIndex + 1;
    int lastCellIndexToRemove = lastCellIndex == INDEX_NONE ? mContainer->GetCells().Num() - 1 : lastCellIndex - 1;
    int numCellsToRemove = lastCellIndexToRemove - firstCellIndexToRemove + 1;
    
    if (numCellsToRemove > 0)
    {
        Remove(firstCellIndexToRemove, numCellsToRemove);
    }

    //Should the first cell in frame range shrink ?
    if (firstCellIndex != INDEX_NONE)
    {
        int cellFrame = mContainer->GetCellFrameAtFrame(startFrame);
        if (firstCellIndex == 0)
        {
            Remove(firstCellIndex);
        }
        else
        {
            SetLength(firstCellIndex, cellFrame);
        }
    }
}

void
FOdysseyAnimationCellsMutator::SetLength(TSharedPtr<FOdysseyAnimationCell> iCell, int iLength)
{
    if (!iCell)
        return;

    if (!mCellMutations.Contains(iCell))
    {
        TSharedPtr<FOdysseySetCellLengthMutation> mutation = MakeShared<FOdysseySetCellLengthMutation>(iCell, iLength, iCell->GetLength());
        mCellMutations.Add(iCell, mutation);
        AddMutation(mCellMutations[iCell]);
    }
    mCellMutations[iCell]->Set(iLength);
    ApplyMutation(mCellMutations[iCell]);
}

void
FOdysseyAnimationCellsMutator::SetLength(int iIndex, int iLength)
{
    if (iIndex < 0 || iIndex >= mContainer->mCells.Num())
        return;

    TSharedPtr<FOdysseyAnimationCell> cell = mContainer->mCells[iIndex];
    SetLength(cell, iLength);
}

void
FOdysseyAnimationCellsMutator::SetMarkId(TSharedPtr<FOdysseyAnimationCell> iCell, const FString& iMarkId)
{
    if (!iCell)
        return;

    TSharedPtr<FOdysseySetCellMarkIdMutation> mutation = MakeShared<FOdysseySetCellMarkIdMutation>(iCell, iMarkId, iCell->GetMarkId());
    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationCellsMutator::SetMarkId(int iIndex, const FString& iMarkId)
{
    if (iIndex < 0 || iIndex >= mContainer->mCells.Num())
        return;

    TSharedPtr<FOdysseyAnimationCell> cell = mContainer->mCells[iIndex];
    SetMarkId(cell, iMarkId);
}

void
FOdysseyAnimationCellsMutator::SetOffset(int iOffset)
{
    if (!mOffsetMutation)
    {
        mOffsetMutation = MakeShared<FOdysseySetCellsOffsetMutation>(mContainer, iOffset, mContainer->GetOffset());
        AddMutation(mOffsetMutation);
    }
    mOffsetMutation->Set(iOffset);
    ApplyMutation(mOffsetMutation);
}

//=======================================================================================

FOdysseyAddCellsMutation::FOdysseyAddCellsMutation(TSharedPtr<FOdysseyAnimationCellsContainer> iContainer, int iIndex, TArray<TSharedPtr<FOdysseyAnimationCell>> iCells)
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

FOdysseyRemoveCellsMutation::FOdysseyRemoveCellsMutation(TSharedPtr<FOdysseyAnimationCellsContainer> iContainer, int iIndex, TArray<TSharedPtr<FOdysseyAnimationCell>> iCells)
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

FOdysseySetCellLengthMutation::FOdysseySetCellLengthMutation(TSharedPtr<FOdysseyAnimationCell> iCell, int iNewLength, int iOldLength)
    : mCell(iCell)
    , mNewLength(iNewLength)
    , mOldLength(iOldLength)
{
}

bool 
FOdysseySetCellLengthMutation::IsDirty() const
{
    return mNewLength != mOldLength;
}

void
FOdysseySetCellLengthMutation::Set(int iNewLength)
{
    mNewLength = iNewLength;
}

void
FOdysseySetCellLengthMutation::Apply()
{
    mCell->mLength = mNewLength;
}

void
FOdysseySetCellLengthMutation::Revert()
{
    mCell->mLength = mOldLength;
}

//=======================================================================================

FOdysseySetCellMarkIdMutation::FOdysseySetCellMarkIdMutation(TSharedPtr<FOdysseyAnimationCell> iCell, const FString& iNewMarkId, const FString& iOldMarkId)
    : mCell(iCell)
    , mNewMarkId(iNewMarkId)
    , mOldMarkId(iOldMarkId)
{
}

void
FOdysseySetCellMarkIdMutation::Apply()
{
    mCell->mMarkId = mNewMarkId;
}

void
FOdysseySetCellMarkIdMutation::Revert()
{
    mCell->mMarkId = mOldMarkId;
}

//=======================================================================================


FOdysseySetCellsOffsetMutation::FOdysseySetCellsOffsetMutation(TSharedPtr<FOdysseyAnimationCellsContainer> iContainer, int iNewOffset, int iOldOffset)
    : mContainer(iContainer)
    , mNewOffset(iNewOffset)
    , mOldOffset(iOldOffset)
{
}

bool 
FOdysseySetCellsOffsetMutation::IsDirty() const
{
    return mNewOffset != mOldOffset;
}

void
FOdysseySetCellsOffsetMutation::Set(int iNewOffset)
{
    mNewOffset = iNewOffset;
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
