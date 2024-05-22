#include "OdysseyAnimationEditorTimeline.h"

#include "OdysseyLayerStack.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineTool.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineSelectionTool.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineMoveTool.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineCutTool.h"
#include "OdysseyKeyState.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"

//Define base frame width to be 50 pixels
#define BASE_FRAMEWIDTH 50.f
#define MIN_ZOOM 0.01f
#define MAX_ZOOM 1.0f
#define ZOOM_STEP 0.08f

FOdysseyAnimationEditorTimeline::~FOdysseyAnimationEditorTimeline()
{
    Finalize();
}

FOdysseyAnimationEditorTimeline::FOdysseyAnimationEditorTimeline(FOdysseyAnimationEditorExtension* iExtension)
    : mExtension(iExtension)
    , mZoom(1.f)
    , mOffset(0.f)
    , mCellsContainer(nullptr)
    , mSelectedTool(EOdysseyTimelineTool::None)
    , mSelectionTool()
    , mMoveTool()
    , mCutTool()
{
}

void 
FOdysseyAnimationEditorTimeline::Initialize()
{
    mSelectedTool = EOdysseyTimelineTool::Selection;
    mSelectionTool = MakeShared<FOdysseyAnimationTimelineSelectionTool>(this);
    mMoveTool = MakeShared<FOdysseyAnimationTimelineMoveTool>(this);
    mCutTool = MakeShared<FOdysseyAnimationTimelineCutTool>(this);

    BindCurrentLayerChanged();
    BindOnCellsChanged();
}

void 
FOdysseyAnimationEditorTimeline::Finalize()
{
    UnbindCurrentLayerChanged();
    UnbindOnCellsChanged();
}

void 
FOdysseyAnimationEditorTimeline::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
    UOdysseyAnimation* animation = mExtension->Animation();
    if (!animation)
        return;

    UOdysseyLayerStack* layerStack = animation->GetLayerStack();
    if (!layerStack)
        return;

    if (iLayerStack != layerStack)
        return;

    UnbindOnCellsChanged();
    BindOnCellsChanged();
}

void 
FOdysseyAnimationEditorTimeline::OnCellsChanged()
{
    CleanSelectedCells();
}

void
FOdysseyAnimationEditorTimeline::CleanSelectedCells()
{
    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mCellsContainer.Pin();
    if (!cellsContainer)
    {
        mSelectedCells.Empty();
        return;
    }

    TArray<TSharedPtr<FOdysseyAnimationCell>> cells = cellsContainer->GetCells();
    mSelectedCells = mSelectedCells.FilterByPredicate(
        [cells](TSharedPtr<FOdysseyAnimationCell> iCell)
        {
            return cells.Contains(iCell);
        }
    );
}

void
FOdysseyAnimationEditorTimeline::CleanCellSelectionCursor()
{
    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mCellsContainer.Pin();
    if (!cellsContainer)
    {
        mCellSelectionCursor = nullptr;
        return;
    }

    TArray<TSharedPtr<FOdysseyAnimationCell>> cells = GetSelectedCells();
    if (cells.IsEmpty())
    {
        mCellSelectionCursor = nullptr;
        return;
    }
    
    if (!cells.Contains(mCellSelectionCursor))
        mCellSelectionCursor = cells[0];
}

void 
FOdysseyAnimationEditorTimeline::BindCurrentLayerChanged()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyAnimationEditorTimeline::OnCurrentLayerChanged);
}

void 
FOdysseyAnimationEditorTimeline::UnbindCurrentLayerChanged()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
}

void 
FOdysseyAnimationEditorTimeline::BindOnCellsChanged()
{
    UOdysseyAnimation* animation = mExtension->Animation();
    if (!animation)
        return;

    UOdysseyLayerStack* layerStack = animation->GetLayerStack();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());    
    if (!currentLayer)
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = currentLayer->GetCellsContainer();
    if (!cellsContainer)
        return;

    
    cellsContainer->OnCellsChanged().AddRaw(this, &FOdysseyAnimationEditorTimeline::OnCellsChanged);
    mCellsContainer = cellsContainer;
}

void 
FOdysseyAnimationEditorTimeline::UnbindOnCellsChanged()
{
    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mCellsContainer.Pin();
    if (!cellsContainer)
        return;

    cellsContainer->OnCellsChanged().RemoveAll(this);
}


TSharedPtr<FOdysseyAnimationTimelineTool>
FOdysseyAnimationEditorTimeline::GetTool() const
{
    switch(GetCurrentTool())
    {
        case EOdysseyTimelineTool::Selection: return mSelectionTool;
        case EOdysseyTimelineTool::Move: return mMoveTool;
        case EOdysseyTimelineTool::Cut: return mCutTool;
    }

    return nullptr;
}

EOdysseyTimelineTool
FOdysseyAnimationEditorTimeline::GetCurrentTool() const
{
    FKey pressedKey = FOdysseyKeyState::GetLastKey();
    if (pressedKey == FKey())
        return mSelectedTool;

    FModifierKeysState modifiers = FSlateApplication::Get().GetModifierKeys();
    const FInputChord activeChord(pressedKey,
        EModifierKey::FromBools(
            modifiers.IsControlDown(),
            modifiers.IsAltDown(),
            modifiers.IsShiftDown(),
            modifiers.IsCommandDown()
        )
    );

    if (FOdysseyAnimationEditorCommands::Get().HoldActivateTimelineSelectionTool->HasActiveChord(activeChord))
    {
        return EOdysseyTimelineTool::Selection;
    }
    else if (FOdysseyAnimationEditorCommands::Get().HoldActivateTimelineMoveTool->HasActiveChord(activeChord))
    {
        return EOdysseyTimelineTool::Move;
    }
    else if (FOdysseyAnimationEditorCommands::Get().HoldActivateTimelineCutTool->HasActiveChord(activeChord))
    {
        return EOdysseyTimelineTool::Cut;
    }

    return mSelectedTool;
}

void
FOdysseyAnimationEditorTimeline::SetSelectedTool(EOdysseyTimelineTool iTool)
{
    if (iTool == mSelectedTool)
        return;

    mSelectedTool = iTool;
    //TODO: Send a ToolChanged event
}

void 
FOdysseyAnimationEditorTimeline::ZoomIn()
{
    SetZoom(FMath::Clamp(mZoom * (1.0f - ZOOM_STEP), MIN_ZOOM, MAX_ZOOM));
}

void 
FOdysseyAnimationEditorTimeline::ZoomOut()
{
    SetZoom(FMath::Clamp(mZoom * (1.0f + ZOOM_STEP), MIN_ZOOM, MAX_ZOOM));
}

void 
FOdysseyAnimationEditorTimeline::SetZoom(float iZoom)
{
    mZoom = FMath::Clamp(iZoom, MIN_ZOOM, MAX_ZOOM);
    mOnZoomChanged.Broadcast();
}

void 
FOdysseyAnimationEditorTimeline::SetOffset(float iOffset)
{
    mOffset = iOffset;
    mOnOffsetChanged.Broadcast();
}

void
FOdysseyAnimationEditorTimeline::SelectCell(TSharedPtr<FOdysseyAnimationCell> iCell, bool iSetAsCursor)
{
    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mCellsContainer.Pin();
    if (!cellsContainer)
    {
        mCellSelectionCursor = nullptr;
        mSelectedCells.Empty();
        return;
    }

    if (!cellsContainer->GetCells().Contains(iCell))
        return;

    if (iSetAsCursor)
        mCellSelectionCursor = iCell;

    if (mSelectedCells.Contains(iCell))
        return;

    mSelectedCells.Add(iCell);
    CleanSelectedCells();
}

void
FOdysseyAnimationEditorTimeline::SetSelectedCells(const TArray<TSharedPtr<FOdysseyAnimationCell>>& iSelectedCells)
{
    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mCellsContainer.Pin();
    if (!cellsContainer)
    {
        mSelectedCells.Empty();
        return;
    }

    mSelectedCells = iSelectedCells;
    
    TArray<TSharedPtr<FOdysseyAnimationCell>> cells = cellsContainer->GetCells();
    mSelectedCells.Sort(
        [cells](TSharedPtr<FOdysseyAnimationCell> iCellA, TSharedPtr<FOdysseyAnimationCell> iCellB)
        {
            return cells.Find(iCellA) < cells.Find(iCellB);
        }
    );
}

//static
float
FOdysseyAnimationEditorTimeline::GetBaseFrameSize()
{
    return BASE_FRAMEWIDTH;
}

float
FOdysseyAnimationEditorTimeline::GetFrameWidth() const
{
    return BASE_FRAMEWIDTH * mZoom;
}

float
FOdysseyAnimationEditorTimeline::GetZoom() const
{
    return mZoom;
}

float
FOdysseyAnimationEditorTimeline::GetOffset() const
{
    return mOffset;
}

int
FOdysseyAnimationEditorTimeline::GetFrameIndexAtMousePosition(float iX) const
{
    int frame = (int)(iX / GetFrameWidth() + mOffset);
    if (frame < 0 )
        return INDEX_NONE;

    return frame;
}

const TArray<TSharedPtr<FOdysseyAnimationCell>>&
FOdysseyAnimationEditorTimeline::GetSelectedCells() const
{
    return mSelectedCells;
}

TSharedPtr<FOdysseyAnimationCell>
FOdysseyAnimationEditorTimeline::GetCellSelectionCursor() const
{
    FOdysseyAnimationEditorTimeline* self = const_cast<FOdysseyAnimationEditorTimeline*>(this);
    self->CleanCellSelectionCursor();
    return mCellSelectionCursor;
}

FSimpleMulticastDelegate&
FOdysseyAnimationEditorTimeline::OnOffsetChanged()
{
    return mOnOffsetChanged;
}

FSimpleMulticastDelegate&
FOdysseyAnimationEditorTimeline::OnZoomChanged()
{
    return mOnZoomChanged;
}

		