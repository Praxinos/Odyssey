#include "OdysseyAnimationEditorTimeline.h"

#include "OdysseyLayerStack.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineTool.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineSelectionTool.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineMoveTool.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineCutTool.h"
#include "OdysseyKeyState.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "OdysseyStyleSet.h"

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
    , mOffset(0)
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
    if (!mLayer)
    {
        mSelectedCells.Empty();
        return;
    }

    TArray<UOdysseyAnimationCell*> cells = mLayer->GetCells();
    mSelectedCells = mSelectedCells.FilterByPredicate(
        [cells](UOdysseyAnimationCell* iCell)
        {
            return cells.Contains(iCell);
        }
    );
}

void
FOdysseyAnimationEditorTimeline::CleanCellSelectionCursor()
{
    if (!mLayer)
    {
        mCellSelectionCursor = nullptr;
        return;
    }

    TArray<UOdysseyAnimationCell*> cells = GetSelectedCells();
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

    mLayer = currentLayer;
    currentLayer->OnCellsChanged().AddRaw(this, &FOdysseyAnimationEditorTimeline::OnCellsChanged);
}

void
FOdysseyAnimationEditorTimeline::UnbindOnCellsChanged()
{
    if (!mLayer)
        return;

    mLayer->OnCellsChanged().RemoveAll(this);
    mLayer = nullptr;
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
    mOffset = FMath::Max(0, iOffset);
    mOnOffsetChanged.Broadcast();
}

void
FOdysseyAnimationEditorTimeline::SelectCell(UOdysseyAnimationCell* iCell, bool iSetAsCursor)
{
    if (!mLayer)
    {
        mCellSelectionCursor = nullptr;
        mSelectedCells.Empty();
        return;
    }

    if (!mLayer->GetCells().Contains(iCell))
        return;

    if (iSetAsCursor)
        mCellSelectionCursor = iCell;

    if (mSelectedCells.Contains(iCell))
        return;

    mSelectedCells.Add(iCell);
    CleanSelectedCells();
}

void
FOdysseyAnimationEditorTimeline::SetSelectedCells(const TArray<UOdysseyAnimationCell*>& iSelectedCells)
{
    if (!mLayer)
    {
        mSelectedCells.Empty();
        return;
    }

    mSelectedCells = iSelectedCells;

    TArray<UOdysseyAnimationCell*> cells = mLayer->GetCells();
    mSelectedCells.Sort(
        [cells](const UOdysseyAnimationCell& iCellA, const UOdysseyAnimationCell& iCellB)
        {
            return iCellA.IndexInLayer < iCellB.IndexInLayer;
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

float
FOdysseyAnimationEditorTimeline::GetPadding() const
{
    return FOdysseyStyle::GetFloat(TEXT("Animation.Timeline.Padding"));
}

float
FOdysseyAnimationEditorTimeline::MousePositionToFrame(float iX) const
{
    float padding = FOdysseyStyle::GetFloat(TEXT("Animation.Timeline.Padding"));
    float frame = (iX - padding) / GetFrameWidth() + mOffset;
    return frame;
}

float
FOdysseyAnimationEditorTimeline::FrameToMousePosition(float iFrame) const
{
    float padding = FOdysseyStyle::GetFloat(TEXT("Animation.Timeline.Padding"));
    float pos = (iFrame - mOffset) * GetFrameWidth() + padding;
    return pos;
}

const TArray<UOdysseyAnimationCell*>&
FOdysseyAnimationEditorTimeline::GetSelectedCells() const
{
    return mSelectedCells;
}

UOdysseyAnimationCell*
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


