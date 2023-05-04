// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationCellsMutator.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"

FOdysseyAnimationCellsMutator::FOdysseyAnimationCellsMutator(UOdysseyAnimationLayerImageRaster* iLayer, bool iGenerateUndo)
    : FOdysseyMutator(iLayer, "FOdysseyAnimationCellsMutator", iGenerateUndo)
    , mLayer(iLayer)
{
    GetRootMutation()->OnMutated().BindLambda(
        [iLayer]()
        {
            iLayer->CellsChanged();
        }
    );
}

void
FOdysseyAnimationCellsMutator::Add(TArray<TSharedPtr<FOdysseyAnimationCell>> iCells, int iIndex)
{
    int index = iIndex < 0 ? mLayer->mCells.Num() : iIndex;
    TSharedPtr<FOdysseyAddCellsMutation> mutation = MakeShared<FOdysseyAddCellsMutation>(mLayer, index, iCells);
    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationCellsMutator::Remove(int iIndex, int iNumCells)
{
    if (iNumCells < 1 || iIndex < 0 || iIndex + iNumCells > mLayer->GetCellsCount())
        return;
        
    TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
    for (int i = 0; i < iNumCells; i++)
    {
        TSharedPtr<FOdysseyAnimationCell> cell = mLayer->GetCell(i + iIndex);
        cells.Add(cell);
    }

    TSharedPtr<FOdysseyRemoveCellsMutation> mutation = MakeShared<FOdysseyRemoveCellsMutation>(mLayer, iIndex, cells);
    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationCellsMutator::SetLength(int iIndex, int iLength)
{
    if (iIndex < 0 || iIndex >= mLayer->GetCellsCount())
        return;

    int oldLength = mLayer->mCells[iIndex]->GetLength();

    TSharedPtr<FOdysseySetCellLengthMutation> mutation = MakeShared<FOdysseySetCellLengthMutation>(mLayer, iIndex, iLength, oldLength);
    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationCellsMutator::SetOffset(int iOffset)
{
    TSharedPtr<FOdysseySetCellsOffsetMutation> mutation = MakeShared<FOdysseySetCellsOffsetMutation>(mLayer, iOffset, mLayer->GetOffset());
    AddAndApplyMutation(mutation);
}

//=======================================================================================

FOdysseyAddCellsMutation::FOdysseyAddCellsMutation(UOdysseyAnimationLayerImageRaster* iLayer, int iIndex, TArray<TSharedPtr<FOdysseyAnimationCell>> iCells)
    : mLayer(iLayer)
    , mIndex(iIndex)
    , mCells(iCells)
{
}

//static
const FGuid&
FOdysseyAddCellsMutation::StaticID()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

const
FGuid&
FOdysseyAddCellsMutation::GetID()
{
    return StaticID();
}

void
FOdysseyAddCellsMutation::Apply()
{
    mLayer->mCells.Insert(mCells, mIndex);
}

void
FOdysseyAddCellsMutation::Revert()
{
    mLayer->mCells.RemoveAt(mIndex, mCells.Num());
}

//=======================================================================================

FOdysseyRemoveCellsMutation::FOdysseyRemoveCellsMutation(UOdysseyAnimationLayerImageRaster* iLayer, int iIndex, TArray<TSharedPtr<FOdysseyAnimationCell>> iCells)
    : mLayer(iLayer)
    , mIndex(iIndex)
    , mCells(iCells)
{
}

//static
const FGuid&
FOdysseyRemoveCellsMutation::StaticID()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

const
FGuid&
FOdysseyRemoveCellsMutation::GetID()
{
    return StaticID();
}

void
FOdysseyRemoveCellsMutation::Apply()
{
    mLayer->mCells.RemoveAt(mIndex, mCells.Num());
}

void
FOdysseyRemoveCellsMutation::Revert()
{
    mLayer->mCells.Insert(mCells, mIndex);
}

//=======================================================================================

FOdysseySetCellLengthMutation::FOdysseySetCellLengthMutation(UOdysseyAnimationLayerImageRaster* iLayer, int iIndex, int iNewLength, int iOldLength)
    : mLayer(iLayer)
    , mIndex(iIndex)
    , mNewLength(iNewLength)
    , mOldLength(iOldLength)
{
}

//static
const FGuid&
FOdysseySetCellLengthMutation::StaticID()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

const
FGuid&
FOdysseySetCellLengthMutation::GetID()
{
    return StaticID();
}

void
FOdysseySetCellLengthMutation::Apply()
{
    mLayer->mCells[mIndex]->SetLength(mNewLength);
}

void
FOdysseySetCellLengthMutation::Revert()
{
    mLayer->mCells[mIndex]->SetLength(mOldLength);
}

//=======================================================================================


FOdysseySetCellsOffsetMutation::FOdysseySetCellsOffsetMutation(UOdysseyAnimationLayerImageRaster* iLayer, int iNewOffset, int iOldOffset)
    : mLayer(iLayer)
    , mNewOffset(iNewOffset)
    , mOldOffset(iOldOffset)
{
}

//static
const FGuid&
FOdysseySetCellsOffsetMutation::StaticID()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

const
FGuid&
FOdysseySetCellsOffsetMutation::GetID()
{
    return StaticID();
}

void
FOdysseySetCellsOffsetMutation::Apply()
{
    mLayer->mOffset = mNewOffset;
}

void
FOdysseySetCellsOffsetMutation::Revert()
{
    mLayer->mOffset = mOldOffset;
}
