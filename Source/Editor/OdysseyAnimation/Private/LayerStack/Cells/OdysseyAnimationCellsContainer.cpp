// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainerExport.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainerImport.h"

FOdysseyAnimationCellsContainer::FOnCellsChanged&
FOdysseyAnimationCellsContainer::OnCellsChanged()
{
    return mOnCellsChanged;
}

FOdysseyAnimationCellsContainer::~FOdysseyAnimationCellsContainer()
{
}

FOdysseyAnimationCellsContainer::FOdysseyAnimationCellsContainer()
    : mOffset(0)
{
}

void
FOdysseyAnimationCellsContainer::AddSupportedType(const FName& iCellId)
{
    mSupportedTypes.Add(iCellId);
}

bool
FOdysseyAnimationCellsContainer::SupportsType(const FName& iCellType) const
{
    return mSupportedTypes.Contains(iCellType);
}

FOdysseyAnimationCellsContainer::FCreateCell&
FOdysseyAnimationCellsContainer::CreateCellDelegate()
{
    return mCreateCell;
}

int
FOdysseyAnimationCellsContainer::GetOffset() const
{
    return mOffset;
}

FInt32Range
FOdysseyAnimationCellsContainer::GetFrameRange() const
{
    uint32 length = 0;
    for (TSharedPtr<FOdysseyAnimationCell> cell : mCells)
    {
        length += cell->GetLength();
    }
    return FInt32Range::Inclusive(mOffset, mOffset + length - 1);
}

const TArray<TSharedPtr<FOdysseyAnimationCell>>&
FOdysseyAnimationCellsContainer::GetCells() const
{
    return mCells;
}

TSharedPtr<FOdysseyAnimationCell>
FOdysseyAnimationCellsContainer::GetCellAtFrame(int iFrameIndex) const
{
    int celIndex = GetCellIndexAtFrame(iFrameIndex);
    if ( celIndex == INDEX_NONE)
        return nullptr;

    return mCells[celIndex];
}

int
FOdysseyAnimationCellsContainer::GetCellIndexAtFrame(int iFrameIndex) const
{
    if( iFrameIndex < mOffset )
        return INDEX_NONE;

    int frameIndex = mOffset;
    for (int i = 0; i < mCells.Num(); i++)
    {
        TSharedPtr<FOdysseyAnimationCell> cell = mCells[i];

        if ( frameIndex + cell->GetLength() - 1 >= iFrameIndex)
            return i;

        frameIndex += cell->GetLength();
    }

    return INDEX_NONE;
}

int
FOdysseyAnimationCellsContainer::GetCellFrameAtFrame(int iFrameIndex) const
{
    if( iFrameIndex < mOffset )
        return INDEX_NONE;

    int frameIndex = mOffset;
    for (int i = 0; i < mCells.Num(); i++)
    {
        TSharedPtr<FOdysseyAnimationCell> cell = mCells[i];

        if ( frameIndex + cell->GetLength() - 1 >= iFrameIndex)
            return iFrameIndex - frameIndex;

        frameIndex += cell->GetLength();
    }

    return INDEX_NONE;
}

FInt32Range
FOdysseyAnimationCellsContainer::GetCellFrameRange(int iIndex) const
{
    if (iIndex < 0 || iIndex >= mCells.Num())
        return FInt32Range::Empty();

    uint32 startFrame = mOffset;
    for (int i = 0; i < iIndex; i++ )
    {
        startFrame += mCells[i]->GetLength();
    }
    return FInt32Range::Inclusive(startFrame, startFrame + mCells[iIndex]->GetLength() - 1);
}

bool
FOdysseyAnimationCellsContainer::HasCellAtFrame(int iFrame) const
{
    return GetCellIndexAtFrame(iFrame) != INDEX_NONE;
}

bool
FOdysseyAnimationCellsContainer::IsCellHeadAtFrame(int iFrame) const
{
    return GetCellFrameAtFrame(iFrame) == 0;
}

int
FOdysseyAnimationCellsContainer::GetCellIndex(TSharedPtr<const FOdysseyAnimationCell> iCell) const
{
    return mCells.Find(ConstCastSharedPtr<FOdysseyAnimationCell>(iCell));
}

int
FOdysseyAnimationCellsContainer::GetCellFrame(TSharedPtr<const FOdysseyAnimationCell> iCell) const
{
    uint32 startFrame = mOffset;
    for (int i = 0; i < mCells.Num(); i++ )
    {
        if (mCells[i] == iCell)
            return startFrame;
        startFrame += mCells[i]->GetLength();
    }
    return INDEX_NONE;
}

void
FOdysseyAnimationCellsContainer::PostDuplicate()
{
    for ( int i = 0; i < mCells.Num(); i++ )
    {
        TSharedPtr<FOdysseyAnimationCell> cell = mCells[i];
        cell->PostDuplicate();
    }
}

void
FOdysseyAnimationCellsContainer::Serialize(FArchive& Ar)
{
    if ( Ar.IsTransacting() || !Ar.IsPersistent() )
        return;

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellsContainerExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellsContainerImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            //Load or Save the offset
            Ar << mOffset;

            //Empty Cells to prepare for loading
            if ( Ar.IsLoading() )
                mCells.Empty();

            //Load or Save number of cells
            int32 numCells = mCells.Num();
            Ar << numCells;

            for ( int i = 0; i < numCells; i++ )
            {
                if ( Ar.IsLoading() )
                {
                    //Load the cell type
                    FName cellType;
                    Ar << cellType;

                    //Create a cell of the given type
                    TSharedPtr<FOdysseyAnimationCell> cell = mCreateCell.Execute(cellType, true);
                    checkf(!!cell, TEXT("Failed to create a cell of the given type"));

                    //Load the cell
                    cell->Serialize(Ar);

                    //Add the cell to the cell list
                    mCells.Add(cell);
                }
                else
                {
                    //Save the Cell Type
                    FName cellType = mCells[i]->GetType();
                    Ar << cellType;

                    //Save the Cell
                    mCells[i]->Serialize(Ar);
                }
            }
        }
    }

    
}

void
FOdysseyAnimationCellsContainer::PostLoad()
{
    for ( int i = 0; i < mCells.Num(); i++ )
    {
        TSharedPtr<FOdysseyAnimationCell> cell = mCells[i];
        cell->PostLoad();
    }
}
