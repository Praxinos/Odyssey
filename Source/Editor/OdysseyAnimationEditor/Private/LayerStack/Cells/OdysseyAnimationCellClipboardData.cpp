// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCellClipboardData.h"

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "UObject/OdysseyObjectEditorUtils.h"

FOdysseyAnimationCellClipboardData::FOdysseyAnimationCellClipboardData()
    : IOdysseyClipboardData(StaticId())
{

}

FOdysseyAnimationCellClipboardData::FOdysseyAnimationCellClipboardData(const TArray<UOdysseyAnimationCell*>& iCells)
    : IOdysseyClipboardData(StaticId())
{
    Copy(iCells);
}

const FGuid&
FOdysseyAnimationCellClipboardData::StaticId()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

void
FOdysseyAnimationCellClipboardData::Copy(const TArray<UOdysseyAnimationCell*>& iCells)
{
    mCellCopies.Empty();
    for (UOdysseyAnimationCell* cell : iCells)
    {
        mCellCopies.Add({cell, cell->Exposure});
    }
}

void
FOdysseyAnimationCellClipboardData::Paste(UOdysseyAnimationLayer* iLayer, int iFrame) const
{
    checkf(CanPaste(iLayer), TEXT("Can't Paste"));

    int cellIndex = INDEX_NONE;

    FInt32Range layerRange = iLayer->GetFrameRange();
    if (iFrame < layerRange.GetLowerBoundValue())
    {
        cellIndex = 0;
    }
    else if (iFrame > layerRange.GetUpperBoundValue())
    {
        cellIndex = iLayer->GetCells().Num();
    }
    else
    {
        UOdysseyAnimationCell* cell = iLayer->GetCellAtFrame(iFrame);
        cellIndex = cell->IndexInLayer;
        int cellFrame = iFrame - cell->GetFrameRange().GetLowerBoundValue();
        if (cellFrame != 0)
        {
            cell->Break(cellFrame);
            cellIndex++;
        }
    }

    for (int i = 0; i < mCellCopies.Num(); i++)
    {
        const FCellCopy& cellCopy = mCellCopies[i];
        UOdysseyAnimationCell* cell = iLayer->CopyCell(cellCopy.mCell, cellIndex + i);
        FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), cellCopy.mExposure);
    }
}

bool
FOdysseyAnimationCellClipboardData::CanPaste(UOdysseyAnimationLayer* iLayer) const
{
    if (iLayer->IsLockedRecursively())
        return false;

    for (const FCellCopy& cellCopy : mCellCopies)
    {
        if (!iLayer->GetSupportedCellTypes().Contains(cellCopy.mCell->GetClass()))
            return false;
    }

    return true;
}

int
FOdysseyAnimationCellClipboardData::GetCellCount() const
{
    return mCellCopies.Num();
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyAnimationCellClipboardData::AddReferencedObjects(FReferenceCollector& Collector)
{
    for (FCellCopy& cellCopy : mCellCopies)
    {
        Collector.AddReferencedObject(cellCopy.mCell);
    }
}

FString
FOdysseyAnimationCellClipboardData::GetReferencerName() const
{
    return "FOdysseyAnimationCellClipboardData";
}
