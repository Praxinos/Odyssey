// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationEditorTimelinePosition.h"
#include "OdysseyStyleSet.h"

FOdysseyAnimationEditorTimelinePosition::FOdysseyAnimationEditorTimelinePosition()
    : mBaseFrameSize(50.f)
    , mOffset(0.f)
    , mZoom(1.f)
    , mMinZoom(0.01f)
    , mMaxZoom(1.0f)
    , mZoomStep(0.08f)
    , mCanZoom(true)
    , mPadding(FOdysseyStyle::GetFloat(TEXT("Animation.Timeline.Padding")))
    , mHasMinZoom(true)
    , mHasMaxZoom(true)
{
}

void
FOdysseyAnimationEditorTimelinePosition::CanZoom(bool iCanZoom)
{
    mCanZoom = false;
}

bool
FOdysseyAnimationEditorTimelinePosition::CanZoom() const
{
    return mCanZoom;
}

void
FOdysseyAnimationEditorTimelinePosition::SetBaseFrameSize(float iFrameSize)
{
    mBaseFrameSize = iFrameSize;
}

float
FOdysseyAnimationEditorTimelinePosition::GetBaseFrameSize() const
{
    return mBaseFrameSize;
}

float
FOdysseyAnimationEditorTimelinePosition::GetFrameSize() const
{
    return mBaseFrameSize * mZoom;
}

void
FOdysseyAnimationEditorTimelinePosition::ZoomIn()
{
    SetZoom(mZoom * (1.0f - mZoomStep));
}

void
FOdysseyAnimationEditorTimelinePosition::ZoomOut()
{
    SetZoom(mZoom * (1.0f + mZoomStep));
}

void
FOdysseyAnimationEditorTimelinePosition::SetZoom(float iZoom)
{
    float zoom = iZoom;
    if (mHasMinZoom)
        zoom = FMath::Max(zoom, mMinZoom);

    if (mHasMaxZoom)
        zoom = FMath::Min(zoom, mMaxZoom);

    mZoom = zoom;
}

void
FOdysseyAnimationEditorTimelinePosition::SetMinZoom(float iZoom)
{
    mMinZoom = iZoom;
    SetZoom(mZoom);
}

void
FOdysseyAnimationEditorTimelinePosition::SetMaxZoom(float iZoom)
{
    mMaxZoom = iZoom;
    SetZoom(mZoom);
}

void
FOdysseyAnimationEditorTimelinePosition::SetZoomStep(float iZoom)
{
    mZoomStep = iZoom;
}

float
FOdysseyAnimationEditorTimelinePosition::GetZoom() const
{
    return mZoom;
}

float
FOdysseyAnimationEditorTimelinePosition::GetMinZoom() const
{
    return mMinZoom;
}

float
FOdysseyAnimationEditorTimelinePosition::GetMaxZoom() const
{
    return mMaxZoom;
}

float
FOdysseyAnimationEditorTimelinePosition::GetZoomStep() const
{
    return mZoomStep;
}

void
FOdysseyAnimationEditorTimelinePosition::SetOffset(float iOffset)
{
    mOffset = FMath::Max(0, iOffset);
}

float
FOdysseyAnimationEditorTimelinePosition::GetOffset() const
{
    return mOffset;
}

float
FOdysseyAnimationEditorTimelinePosition::MousePositionToFrame(float iX) const
{
    float frame = (iX - mPadding) / GetFrameSize() + mOffset;
    return frame;
}

float
FOdysseyAnimationEditorTimelinePosition::FrameToMousePosition(float iFrame) const
{
    float pos = (iFrame - mOffset) * GetFrameSize() + mPadding;
    return pos;
}

void
FOdysseyAnimationEditorTimelinePosition::SetPadding(float iPadding)
{
    mPadding = iPadding;
}

float
FOdysseyAnimationEditorTimelinePosition::GetPadding() const
{
    return mPadding;
}

bool
FOdysseyAnimationEditorTimelinePosition::HasMinZoom() const
{
    return mHasMinZoom;
}

void
FOdysseyAnimationEditorTimelinePosition::HasMinZoom(bool iHasMinZoom)
{
    mHasMinZoom = iHasMinZoom;
}

bool
FOdysseyAnimationEditorTimelinePosition::HasMaxZoom() const
{
    return mHasMaxZoom;
}

void
FOdysseyAnimationEditorTimelinePosition::HasMaxZoom(bool iHasMaxZoom)
{
    mHasMaxZoom = iHasMaxZoom;
}
