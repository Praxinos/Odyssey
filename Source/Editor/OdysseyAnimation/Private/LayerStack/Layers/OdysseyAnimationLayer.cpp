// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/OdysseyAnimationLayer.h"

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayerImageRenderer.h"
#include "ULISLoaderModule.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"

//===========================

void
UOdysseyAnimationLayer::PostInitProperties()
{
	Super::PostInitProperties();

	//Activate first previous and first next keys in the lighttable by default
	Lighttable.PreviousKeys[0].IsActivated = true;
	Lighttable.NextKeys[0].IsActivated = true;
}

void
UOdysseyAnimationLayer::InvalidateCellFrameRanges()
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

    return FInt32Range::Hull(ranges);
}

FSimpleMulticastDelegate&
UOdysseyAnimationLayer::OnLightTableChanged()
{
    return mOnLightTableChanged;
}

void
UOdysseyAnimationLayer::LightTableChanged()
{   
    ImageRenderingCompositionChanged();
    OnLightTableChanged().Broadcast();
}

void
UOdysseyAnimationLayer::CellsChanged()
{
	UpdateCellsIndexInLayer();
	InvalidateCellFrameRanges();
    ImageRenderingCompositionChanged();
    UOdysseyLayer::OnMediaChanged().Broadcast();
}

void
UOdysseyAnimationLayer::CellsOffsetChanged()
{
	InvalidateCellFrameRanges();
    ImageRenderingCompositionChanged();
	UOdysseyLayer::OnMediaChanged().Broadcast();
}

void
UOdysseyAnimationLayer::PreBehaviourChanged()
{
    ImageRenderingCompositionChanged();
}

void
UOdysseyAnimationLayer::PostBehaviourChanged()
{
    ImageRenderingCompositionChanged();
}

void
UOdysseyAnimationLayer::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
    Super::PropertyChanged(iPropertyName, iMemberPropertyName, iIsInteractive);

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PreBehaviour))
        PreBehaviourChanged();
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PostBehaviour))
        PostBehaviourChanged();
    if (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, LightTable))
        LightTableChanged();
	if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells))
        CellsChanged();
	if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset))
        CellsOffsetChanged();
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

const TArray<TObjectPtr<UClass>>&
UOdysseyAnimationLayer::GetSupportedCellTypes() const
{
	return SupportedCellTypes;
}

const TArray<TObjectPtr<UOdysseyAnimationCell>>&
UOdysseyAnimationLayer::GetCells() const
{
	return Cells;
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

        if ( frameIndex + cell->Length - 1 >= Frame)
            return i;

        frameIndex += cell->Length;
    }

    return INDEX_NONE;
}

bool
UOdysseyAnimationLayer::HasCellAtFrame(int Frame) const
{
	return GetCellIndexAtFrame(Frame) != INDEX_NONE;
}

TArray<FInt32Range>
UOdysseyAnimationLayer::GetCellsFrameRanges() const
{
	if (mCellsFrameRanges.Num() != Cells.Num())
	{
		mCellsFrameRanges.Empty();
		uint32 startFrame = CellsOffset;
		for (TObjectPtr<UOdysseyAnimationCell> cell : Cells)
		{
			FInt32Range frameRange = FInt32Range::Inclusive(startFrame, startFrame + cell->Length - 1);
			mCellsFrameRanges.Add(frameRange);
			startFrame += cell->Length;
		}
	}
	
    return mCellsFrameRanges;
}

TArray<UOdysseyAnimationCell*>
UOdysseyAnimationLayer::AddCells(TSubclassOf<UOdysseyAnimationCell*> CellType, int Index = -1, int Count = 0)
{
	UClass* cellType = CellType.Get();

    //No cellType
	if ( !cellType )
		return {};

    //cellType Not supported
    if (!SupportedCellTypes.Contains(cellType))
        return {};

	TArray<UOdysseyAnimationCell*> cells;
	for (int i = 0; i < Count; i++)
	{
		//Create the Layer
		UOdysseyLayer* cell = NewObject<UOdysseyLayer>(this, cellType, NAME_None, RF_Public | RF_Transactional);
		if (!cell )
			return cells;

		cell->Init(this);
		cells.Add(cell);
	}

	FOdysseyObjectEditorUtils::PreChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(Cells));
	Cells.Insert(cells, mIndex);
	FOdysseyObjectEditorUtils::PostChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(Cells), EPropertyChangeType::ArrayAdd);
}

void
UOdysseyAnimationLayer::RemoveCell(UOdysseyAnimationCell* Cell)
{
	RemoveCells({Cell});
	checkf(cell->Parent == this, TEXT("Layer does not contain the cell to remove"));
}

void
UOdysseyAnimationLayer::RemoveCells(TArray<UOdysseyAnimationCell*> Cells)
{
	TArray<UOdysseyAnimationCell*> cells;
	for (UOdysseyAnimationCell* cell : Cells)
	{
		if (!ensureMsgf(cell->Parent == this, TEXT("Layer does not contain the cell to remove")))
			continue;

		FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, IndexInLayer), INDEX_NONE);
		cells.Add(cell);
	}

	FOdysseyObjectEditorUtils::PreChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(Cells));
	Cells.RemoveAll(cells);
	FOdysseyObjectEditorUtils::PostChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(Cells), EPropertyChangeType::ArrayRemove);
}

void
UOdysseyAnimationLayer::RemoveCellAtIndex(int Index)
{
	if (Index < 0 || Index >= Cells.Num())
		return;

	FOdysseyObjectEditorUtils::SetPropertyValue(Cells[Index], GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, IndexInLayer), INDEX_NONE);

	FOdysseyObjectEditorUtils::PreChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(Cells));
	Cells.RemoveAt(Index);
	FOdysseyObjectEditorUtils::PostChangePropertyValue(this, GET_MEMBER_NAME_CHECKED(Cells), EPropertyChangeType::ArrayRemove);
}

TArray<FGuid>
UOdysseyAnimationLayer::GetLighttableImageRenderingComposition(int iFrameIndex) const
{
	int cellIndex = cellsContainer->GetCellIndexAtFrame(iFrameIndex);
	if (cellIndex == INDEX_NONE)
		continue;

    TArray<FGuid> idComposition = {};
    for (int i = 9; i >= 0; i--)
    {
        if (Lighttable.PreviousKeys[i].IsActivated)
		{
			int keyCellIndex = cellIndex - i - 1;
			if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
			{
				UOdysseyAnimationCell* cell = GetCells()[keyCellIndex];
				idComposition.Append(cell->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, 0));
			}
		}

		if (Lighttable.NextKeys[i].IsActivated)
		{
			int keyCellIndex = cellIndex + i + 1;
			if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
			{
				UOdysseyAnimationCell* cell = GetCells()[keyCellIndex];
				idComposition.Append(cell->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, 0));
			}
		}
    }

    return idComposition;
}

void
UOdysseyAnimationLayer::UpdateCellsIndexInLayer()
{
	for (int i = 0, i < Cells.Num(); i++)
	{
		Cells[i].IndexInLayer = i;
	}
}