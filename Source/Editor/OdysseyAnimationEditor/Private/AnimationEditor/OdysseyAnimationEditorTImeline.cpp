#include "OdysseyAnimationEditorTimeline.h"

#include "Framework/Commands/GenericCommands.h"

//Define base frame width to be 50 pixels
#define BASE_FRAMEWIDTH 50.f
#define MIN_ZOOM 0.01f
#define MAX_ZOOM 1.0f
#define ZOOM_STEP 0.08f

FOdysseyAnimationEditorTimeline::FOdysseyAnimationEditorTimeline()
    : mZoom(1.f)
    , mOffset(0.f)
{
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
    mSelectedFrames = iSelectedFrames;
    mOnSelectedFramesChanged.Broadcast();
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

		