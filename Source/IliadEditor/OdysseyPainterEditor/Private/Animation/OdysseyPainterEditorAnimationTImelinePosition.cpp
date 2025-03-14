// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyStyle.h"

FOdysseyPainterEditorAnimationTimelinePosition::FOdysseyPainterEditorAnimationTimelinePosition()
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
FOdysseyPainterEditorAnimationTimelinePosition::CanZoom(bool iCanZoom)
{
    mCanZoom = false;
}

bool
FOdysseyPainterEditorAnimationTimelinePosition::CanZoom() const
{
    return mCanZoom;
}

void
FOdysseyPainterEditorAnimationTimelinePosition::SetBaseFrameSize(float iFrameSize)
{
    mBaseFrameSize = iFrameSize;
}

float
FOdysseyPainterEditorAnimationTimelinePosition::GetBaseFrameSize() const
{
    return mBaseFrameSize;
}

float
FOdysseyPainterEditorAnimationTimelinePosition::GetFrameSize() const
{
    return mBaseFrameSize * mZoom;
}

void
FOdysseyPainterEditorAnimationTimelinePosition::ZoomIn()
{
    SetZoom(mZoom * (1.0f - mZoomStep));
}

void
FOdysseyPainterEditorAnimationTimelinePosition::ZoomOut()
{
    SetZoom(mZoom * (1.0f + mZoomStep));
}

void
FOdysseyPainterEditorAnimationTimelinePosition::SetZoom(float iZoom)
{
    float zoom = iZoom;
    if (mHasMinZoom)
        zoom = FMath::Max(zoom, mMinZoom);

    if (mHasMaxZoom)
        zoom = FMath::Min(zoom, mMaxZoom);

    mZoom = zoom;
}

void
FOdysseyPainterEditorAnimationTimelinePosition::SetMinZoom(float iZoom)
{
    mMinZoom = iZoom;
    SetZoom(mZoom);
}

void
FOdysseyPainterEditorAnimationTimelinePosition::SetMaxZoom(float iZoom)
{
    mMaxZoom = iZoom;
    SetZoom(mZoom);
}

void
FOdysseyPainterEditorAnimationTimelinePosition::SetZoomStep(float iZoom)
{
    mZoomStep = iZoom;
}

float
FOdysseyPainterEditorAnimationTimelinePosition::GetZoom() const
{
    return mZoom;
}

float
FOdysseyPainterEditorAnimationTimelinePosition::GetMinZoom() const
{
    return mMinZoom;
}

float
FOdysseyPainterEditorAnimationTimelinePosition::GetMaxZoom() const
{
    return mMaxZoom;
}

float
FOdysseyPainterEditorAnimationTimelinePosition::GetZoomStep() const
{
    return mZoomStep;
}

void
FOdysseyPainterEditorAnimationTimelinePosition::SetOffset(float iOffset)
{
    mOffset = FMath::Max(0, iOffset);
}

float
FOdysseyPainterEditorAnimationTimelinePosition::GetOffset() const
{
    return mOffset;
}

void
FOdysseyPainterEditorAnimationTimelinePosition::SetPadding(float iPadding)
{
    mPadding = iPadding;
}

float
FOdysseyPainterEditorAnimationTimelinePosition::GetPadding() const
{
    return mPadding;
}

bool
FOdysseyPainterEditorAnimationTimelinePosition::HasMinZoom() const
{
    return mHasMinZoom;
}

void
FOdysseyPainterEditorAnimationTimelinePosition::HasMinZoom(bool iHasMinZoom)
{
    mHasMinZoom = iHasMinZoom;
}

bool
FOdysseyPainterEditorAnimationTimelinePosition::HasMaxZoom() const
{
    return mHasMaxZoom;
}

void
FOdysseyPainterEditorAnimationTimelinePosition::HasMaxZoom(bool iHasMaxZoom)
{
    mHasMaxZoom = iHasMaxZoom;
}

void
FOdysseyPainterEditorAnimationTimelinePosition::Reset()
{
    *this = FOdysseyPainterEditorAnimationTimelinePosition();
}
