#include "OdysseyAnimationEditorTimeline.h"

#include "OdysseyLayerStack.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineTool.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineSelectionTool.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineMoveTool.h"

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
    , mSelectedFrames(FInt32Range::Empty())
    , mCellsContainer(nullptr)
    , mSelectedTool(EOdysseyTimelineTool::None)
    , mSelectionTool()
    , mMoveTool()
{
}

void 
FOdysseyAnimationEditorTimeline::Initialize()
{
    mSelectedTool = EOdysseyTimelineTool::Selection;
    mSelectionTool = MakeShared<FOdysseyAnimationTimelineSelectionTool>(this);
    mMoveTool = MakeShared<FOdysseyAnimationTimelineMoveTool>(this);

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
    SetSelectedFrames(FInt32Range::Empty());
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
    switch(mSelectedTool)
    {
        case EOdysseyTimelineTool::Selection: return mSelectionTool;
        case EOdysseyTimelineTool::Move: return mMoveTool;
    }

    return nullptr;
}

EOdysseyTimelineTool
FOdysseyAnimationEditorTimeline::GetSelectedTool() const
{
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
    mZoom = iZoom;
    mOnZoomChanged.Broadcast();
}

void 
FOdysseyAnimationEditorTimeline::SetOffset(float iOffset)
{
    mOffset = iOffset;
    mOnOffsetChanged.Broadcast();
}

void
FOdysseyAnimationEditorTimeline::SetSelectedFrames(const FInt32Range& iSelectedFrames)
{
    mSelectedFrames = FInt32Range::Intersection(iSelectedFrames, GetSelectableFrames());
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

FInt32Range
FOdysseyAnimationEditorTimeline::GetSelectedFrames() const
{
    return mSelectedFrames;
}

FInt32Range
FOdysseyAnimationEditorTimeline::GetSelectableFrames() const
{
    UOdysseyAnimation* animation = mExtension->Animation();
    if (!animation)
        return FInt32Range::Empty();

    UOdysseyLayerStack* layerStack = animation->GetLayerStack();
    if (!layerStack)
        return FInt32Range::Empty();

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());    
    if (!currentLayer)
        return FInt32Range::Empty();

    return currentLayer->GetFrameRange();
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

		