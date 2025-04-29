// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/OdysseyAnimationLayer.h"

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "ULISLoaderModule.h"

//===========================

FSimpleMulticastDelegate&
UOdysseyAnimationLayer::OnCellsChanged()
{
    return mOnCellsChanged;
}

void
UOdysseyAnimationLayer::PostInitProperties()
{
    Super::PostInitProperties();

    //Activate first previous and first next keys in the lighttable by default
    Lighttable.PreviousKeys[0].bIsActivated = true;
    Lighttable.NextKeys[0].bIsActivated = true;
}

void
UOdysseyAnimationLayer::InvalidateCellsFrameRanges()
{
    mCellsFrameRanges.Empty();
}

UOdysseyAnimation*
UOdysseyAnimationLayer::GetAnimation() const
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());
    if(!layerStack)
        return nullptr;

    return layerStack->GetAnimation();
}

UOdysseyAnimationLayerStack*
UOdysseyAnimationLayer::GetLayerStack() const
{
    return Cast<UOdysseyAnimationLayerStack>(UOdysseyLayer::GetLayerStack());
}

FInt32Range
UOdysseyAnimationLayer::GetFrameRange() const
{
    TArray<FInt32Range> ranges;
    const TArray<UOdysseyLayer*> layers = Children;
    for (UOdysseyLayer* layer : layers)
    {
        UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
        if (!animationLayer)
            continue;

        ranges.Add(animationLayer->GetFrameRange());
    }

    if (!Cells.IsEmpty())
    {
        const TArray<FInt32Range>& cellFrameRanges = GetCellsFrameRanges();
        ranges.Add(FInt32Range::Inclusive( cellFrameRanges[0].GetLowerBoundValue(), cellFrameRanges.Last().GetUpperBoundValue() ));
    }

    return FInt32Range::Hull(ranges);
}

FSimpleMulticastDelegate&
UOdysseyAnimationLayer::OnLightTableChanged()
{
    return mOnLightTableChanged;
}

void
UOdysseyAnimationLayer::LightTableChanged(bool iIsInteractive)
{
}

void
UOdysseyAnimationLayer::CellsChanged(bool iIsInteractive)
{
    UpdateCellsIndexInLayer();
    InvalidateCellsFrameRanges();
}

void
UOdysseyAnimationLayer::CellsOffsetChanged(bool iIsInteractive)
{
    InvalidateCellsFrameRanges();
}

void
UOdysseyAnimationLayer::PreBehaviourChanged()
{
}

void
UOdysseyAnimationLayer::PostBehaviourChanged()
{
}

void
UOdysseyAnimationLayer::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
    Super::PropertyChanged(iPropertyName, iMemberPropertyName, iIsInteractive);

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PreBehaviour))
        PreBehaviourChanged();
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PostBehaviour))
        PostBehaviourChanged();
    if (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable))
        LightTableChanged(iIsInteractive);
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells))
        CellsChanged(iIsInteractive);
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset))
        CellsOffsetChanged(iIsInteractive);
}

void
UOdysseyAnimationLayer::PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive)
{
    Super::PostPropertyChanged(iPropertyName, iIsInteractive);

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PreBehaviour))
    {
        RenderingCompositionChanged();
    }
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PostBehaviour))
    {
        RenderingCompositionChanged();
    }
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable))
    {
        RenderingCompositionChanged(iIsInteractive); //Composition could change if lighttable or a key is activated/inactivated
        RenderingChanged(iIsInteractive); //ImageRendering changes without a composition change when any other param is changed
        OnLightTableChanged().Broadcast();
    }
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells))
    {
        mOnCellsChanged.Broadcast();
        RenderingCompositionChanged(iIsInteractive);
        UOdysseyLayer::OnMediaChanged().Broadcast();
    }
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset))
    {
        RenderingCompositionChanged(iIsInteractive);
        UOdysseyLayer::OnMediaChanged().Broadcast();
    }
}

int
UOdysseyAnimationLayer::GetPreBehaviourFrame(EOdysseyAnimationLayerImagePostBehaviour iBehaviour, int iFrame) const
{
    FInt32Range frameRange = GetFrameRange();

    //PreBehaviour
    int frame = iFrame;
    switch(iBehaviour)
    {
        default:
        case EOdysseyAnimationLayerImagePostBehaviour::None:
        break;

        case EOdysseyAnimationLayerImagePostBehaviour::Hold:
        {
            frame = frameRange.GetLowerBoundValue();
        }
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::Loop:
        {
            int offsetFromStart = frameRange.GetLowerBoundValue() - iFrame;
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;
            frame = frameRange.GetUpperBoundValue() - ((offsetFromStart - 1) % layerLength);
        }
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::PingPong:
        {
            int offsetFromStart = frameRange.GetLowerBoundValue() - iFrame;
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;

            if (layerLength <= 1)
            {
                frame = frameRange.GetLowerBoundValue();
                break;
            }

            int offset = ((offsetFromStart - 1) % (layerLength - 1)) + 1;
            int forwardFrame = frameRange.GetLowerBoundValue() + offset;
            int backwardFrame = frameRange.GetUpperBoundValue() - offset;

            int direction = layerLength > 0 ? ((offsetFromStart - 1) / (layerLength - 1)) % 2 : 0;
            frame = direction == 0 ? forwardFrame : backwardFrame;
        }
        break;
    }

    return frame;
}

int
UOdysseyAnimationLayer::GetPostBehaviourFrame(EOdysseyAnimationLayerImagePostBehaviour iBehaviour, int iFrame) const
{
    //PreBehaviour
    FInt32Range frameRange = GetFrameRange();

    int frame = iFrame;
    switch(iBehaviour)
    {
        default:
        case EOdysseyAnimationLayerImagePostBehaviour::None:
        break;

        case EOdysseyAnimationLayerImagePostBehaviour::Hold:
        {
            frame = frameRange.GetUpperBoundValue();
        }
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::Loop:
        {
            int offsetFromEnd = iFrame - frameRange.GetUpperBoundValue();
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;
            frame = frameRange.GetLowerBoundValue() + (offsetFromEnd - 1) % layerLength;
        }
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::PingPong:
        {
            int offsetFromEnd = iFrame - frameRange.GetUpperBoundValue();
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;

            if (layerLength <= 1)
            {
                frame = frameRange.GetLowerBoundValue();
                break;
            }

            int offset = ((offsetFromEnd - 1) % (layerLength - 1)) + 1;
            int forwardFrame = frameRange.GetLowerBoundValue() + offset;
            int backwardFrame = frameRange.GetUpperBoundValue() - offset;

            int direction = layerLength > 0 ? ((offsetFromEnd - 1) / (layerLength - 1)) % 2 : 0;
            frame = direction == 0 ? backwardFrame : forwardFrame;
        }
        break;
    }

    return frame;
}

const TArray<TSubclassOf<UOdysseyAnimationCell>>&
UOdysseyAnimationLayer::GetSupportedCellTypes() const
{
    return SupportedCellTypes;
}

const TArray<UOdysseyAnimationCell*>&
UOdysseyAnimationLayer::GetCells() const
{
    return Cells;
}

UOdysseyAnimationCell*
UOdysseyAnimationLayer::GetCellAtFrame(int Frame) const
{
    int cellIndex = GetCellIndexAtFrame(Frame);
    if (cellIndex == INDEX_NONE)
        return nullptr;

    return Cells[cellIndex];
}

int
UOdysseyAnimationLayer::GetCellIndexAtFrame(int Frame) const
{
    if( Frame < CellsOffset )
        return INDEX_NONE;

    int frameIndex = CellsOffset;
    for (int i = 0; i < Cells.Num(); i++)
    {
        UOdysseyAnimationCell* cell = Cells[i];
        int cellExposure = 1;
        if (cell)
            cellExposure = cell->Exposure;


        if ( frameIndex + cellExposure - 1 >= Frame)
            return i;

        frameIndex += cellExposure;
    }

    return INDEX_NONE;
}

bool
UOdysseyAnimationLayer::HasCellAtFrame(int Frame) const
{
    return !!GetCellAtFrame(Frame);
}

const TArray<FInt32Range>&
UOdysseyAnimationLayer::GetCellsFrameRanges() const
{
    if (mCellsFrameRanges.Num() != Cells.Num())
    {
        mCellsFrameRanges.Empty();
        uint32 startFrame = CellsOffset;
        for (TObjectPtr<UOdysseyAnimationCell> cell : Cells)
        {
            int cellExposure = 1;
            if (cell)
                cellExposure = cell->Exposure;

            FInt32Range frameRange = FInt32Range::Inclusive(startFrame, startFrame + cellExposure - 1);
            mCellsFrameRanges.Add(frameRange);
            startFrame += cellExposure;
        }
    }

    return mCellsFrameRanges;
}

void
UOdysseyAnimationLayer::AddNullCell(int Index)
{
    AddNullCells(Index);
}

void
UOdysseyAnimationLayer::AddNullCells(int Index, int Count)
{
    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    TArray<UOdysseyAnimationCell*> cells;
    for (int i = 0; i < Count; i++)
    {
        //Create the Layer
        cells.Add(nullptr);
    }

    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells));
    Cells.Insert(cells, Index);
    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells), EPropertyChangeType::ArrayAdd);
}

UOdysseyAnimationCell*
UOdysseyAnimationLayer::AddCell(TSubclassOf<UOdysseyAnimationCell> CellType, int Index)
{
    TArray<UOdysseyAnimationCell*> cells = AddCells(CellType, Index);
    if (cells.IsEmpty())
        return nullptr;

    return cells[0];
}

TArray<UOdysseyAnimationCell*>
UOdysseyAnimationLayer::AddCells(TSubclassOf<UOdysseyAnimationCell> CellType, int Index, int Count)
{
    UClass* cellType = CellType.Get();

    //No cellType
    if ( !cellType )
        return {};

    //cellType Not supported
    if (!SupportedCellTypes.Contains(cellType))
        return {};

    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    TArray<UOdysseyAnimationCell*> cells;
    for (int i = 0; i < Count; i++)
    {
        //Create the Layer
        UOdysseyAnimationCell* cell = NewObject<UOdysseyAnimationCell>(this, cellType, NAME_None, RF_Public | RF_Transactional);
        if (!cell )
            return cells;

        cells.Add(cell);
    }

    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells));
    Cells.Insert(cells, Index);
    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells), EPropertyChangeType::ArrayAdd);

    return cells;
}

void
UOdysseyAnimationLayer::RemoveCell(UOdysseyAnimationCell* Cell)
{
    if (!Cell) //if you need to remove a null cell use RemoveCellAtIndex
        return;

    RemoveCells({Cell});
    checkf(Cell->GetLayer() == this, TEXT("Layer does not contain the cell to remove"));
}

void
UOdysseyAnimationLayer::RemoveCells(const TArray<UOdysseyAnimationCell*>& iCells)
{
    TArray<UOdysseyAnimationCell*> cells;
    for (UOdysseyAnimationCell* cell : iCells)
    {
        if (!cell) //if you need to remove a null cell use RemoveCellAtIndex
            continue;

        if (!ensureMsgf(cell->GetLayer() == this, TEXT("Layer does not contain the cell to remove")))
            continue;

        FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, IndexInLayer), INDEX_NONE);
        cells.Add(cell);
    }

    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells));
    Cells.RemoveAll([&](UOdysseyAnimationCell* iCell) { return cells.Contains(iCell); });
    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells), EPropertyChangeType::ArrayRemove);
}

void
UOdysseyAnimationLayer::RemoveCellAtIndex(int Index)
{
    if (Index < 0 || Index >= Cells.Num())
        return;

    if (Cells[Index])
        FOdysseyObjectEditorUtils::SetPropertyValue(Cells[Index], GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, IndexInLayer), INDEX_NONE);

    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells));
    Cells.RemoveAt(Index);
    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells), EPropertyChangeType::ArrayRemove);
}

TArray<FGuid>
UOdysseyAnimationLayer::GetLighttableImageRenderingComposition(int iFrameIndex) const
{
    UOdysseyAnimationCell* cell = GetCellAtFrame(iFrameIndex);
    if (!cell)
        return {};

    TArray<FGuid> idComposition = {};
    for (int i = 9; i >= 0; i--)
    {
        if (Lighttable.PreviousKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->IndexInLayer - i - 1;
            if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
            {
                UOdysseyAnimationCell* keyCell = GetCells()[keyCellIndex];
                if (keyCell)
                    idComposition.Append(keyCell->GetRenderingComposition(EOdysseyRenderingType::Render, 0));
            }
        }

        if (Lighttable.NextKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->IndexInLayer + i + 1;
            if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
            {
                UOdysseyAnimationCell* keyCell = GetCells()[keyCellIndex];
                if (!keyCell)
                    idComposition.Append(keyCell->GetRenderingComposition(EOdysseyRenderingType::Render, 0));
            }
        }
    }

    return idComposition;
}

void
UOdysseyAnimationLayer::UpdateCellsIndexInLayer()
{
    for (int i = 0; i < Cells.Num(); i++)
    {
        if (!Cells[i])
            continue;

        FOdysseyObjectEditorUtils::SetPropertyValue(Cells[i], GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, IndexInLayer), i);
    }
}

UOdysseyAnimationCell*
UOdysseyAnimationLayer::CopyCell(UOdysseyAnimationCell* Cell, int Index)
{
    //No Layer
    if(!Cell)
        return nullptr;

    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    //Duplicate the cell
    FObjectDuplicationParameters params(Cell, this);
    UOdysseyAnimationCell* cellCopy = Cast<UOdysseyAnimationCell>(StaticDuplicateObjectEx(params));

    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells));
    Cells.Insert(cellCopy, Index);
    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells), EPropertyChangeType::ArrayAdd);

    return cellCopy;
}

TArray<UOdysseyAnimationCell*>
UOdysseyAnimationLayer::CopyCells(TArray<UOdysseyAnimationCell*> iCells, int Index)
{
    TArray<UOdysseyAnimationCell*> cellCopies;

    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    //Sanitize Layers array
    iCells.RemoveAll(
        [this](const UOdysseyAnimationCell* iCell)
        {
            return !iCell;
        }
    );

    //No Layers
    if (iCells.IsEmpty())
        return cellCopies;

    for (UOdysseyAnimationCell* cell : iCells)
    {
        FObjectDuplicationParameters params(cell, this);
        UOdysseyAnimationCell* cellCopy = Cast<UOdysseyAnimationCell>(StaticDuplicateObjectEx(params));
        cellCopies.Add(cellCopy);
    }

    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells));
    Cells.Insert(cellCopies, Index);
    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells), EPropertyChangeType::ArrayAdd);

    return cellCopies;
}

void
UOdysseyAnimationLayer::CellsOffsetBlueprintSetter(int Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), Value);
}

void
UOdysseyAnimationLayer::PreBehaviourBlueprintSetter(EOdysseyAnimationLayerImagePostBehaviour Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PreBehaviour), Value);
}

void
UOdysseyAnimationLayer::PostBehaviourBlueprintSetter(EOdysseyAnimationLayerImagePostBehaviour Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PostBehaviour), Value);
}

void
UOdysseyAnimationLayer::LighttableBlueprintSetter(FOdysseyAnimationLightTable Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), Value);
}

#ifdef WITH_EDITOR

TArray<FName>
UOdysseyAnimationLayer::GetRows() const
{
    TArray<FName> rows = UOdysseyLayer::GetRows();
    rows.Add("Lighttable");
    rows.Add("OutOfPegs");

    return rows;
}

int
UOdysseyAnimationLayer::GetRowHeight(FName iSubRowName) const
{
    if (iSubRowName == "Lighttable")
        return 40;

    if (iSubRowName == "OutOfPegs")
        return 20;

    return Super::GetRowHeight(iSubRowName);
}

bool
UOdysseyAnimationLayer::IsRowVisible(FName iSubRowName) const
{
    if (iSubRowName == "Lighttable")
        return DisplayOptions && HasLighttable && Lighttable.bIsActivated;

    if (iSubRowName == "OutOfPegs")
        return DisplayOptions && HasLighttable && Lighttable.bIsActivated;

    return Super::IsRowVisible(iSubRowName);
}

#endif
