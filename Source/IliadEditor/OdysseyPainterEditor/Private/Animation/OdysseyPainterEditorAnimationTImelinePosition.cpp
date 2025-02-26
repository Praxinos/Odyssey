// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorAnimationTImelinePosition.h"
#include "OdysseyStyle.h"

FOdysseyPainterEditorAnimationTImelinePosition::FOdysseyPainterEditorAnimationTImelinePosition()
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
FOdysseyPainterEditorAnimationTImelinePosition::CanZoom(bool iCanZoom)
{
    mCanZoom = false;
}

bool
FOdysseyPainterEditorAnimationTImelinePosition::CanZoom() const
{
    return mCanZoom;
}

void
FOdysseyPainterEditorAnimationTImelinePosition::SetBaseFrameSize(float iFrameSize)
{
    mBaseFrameSize = iFrameSize;
}

float
FOdysseyPainterEditorAnimationTImelinePosition::GetBaseFrameSize() const
{
    return mBaseFrameSize;
}

float
FOdysseyPainterEditorAnimationTImelinePosition::GetFrameSize() const
{
    return mBaseFrameSize * mZoom;
}

void
FOdysseyPainterEditorAnimationTImelinePosition::ZoomIn()
{
    SetZoom(mZoom * (1.0f - mZoomStep));
}

void
FOdysseyPainterEditorAnimationTImelinePosition::ZoomOut()
{
    SetZoom(mZoom * (1.0f + mZoomStep));
}

void
FOdysseyPainterEditorAnimationTImelinePosition::SetZoom(float iZoom)
{
    float zoom = iZoom;
    if (mHasMinZoom)
        zoom = FMath::Max(zoom, mMinZoom);

    if (mHasMaxZoom)
        zoom = FMath::Min(zoom, mMaxZoom);

    mZoom = zoom;
}

void
FOdysseyPainterEditorAnimationTImelinePosition::SetMinZoom(float iZoom)
{
    mMinZoom = iZoom;
    SetZoom(mZoom);
}

void
FOdysseyPainterEditorAnimationTImelinePosition::SetMaxZoom(float iZoom)
{
    mMaxZoom = iZoom;
    SetZoom(mZoom);
}

void
FOdysseyPainterEditorAnimationTImelinePosition::SetZoomStep(float iZoom)
{
    mZoomStep = iZoom;
}

float
FOdysseyPainterEditorAnimationTImelinePosition::GetZoom() const
{
    return mZoom;
}

float
FOdysseyPainterEditorAnimationTImelinePosition::GetMinZoom() const
{
    return mMinZoom;
}

float
FOdysseyPainterEditorAnimationTImelinePosition::GetMaxZoom() const
{
    return mMaxZoom;
}

float
FOdysseyPainterEditorAnimationTImelinePosition::GetZoomStep() const
{
    return mZoomStep;
}

void
FOdysseyPainterEditorAnimationTImelinePosition::SetOffset(float iOffset)
{
    mOffset = FMath::Max(0, iOffset);
}

float
FOdysseyPainterEditorAnimationTImelinePosition::GetOffset() const
{
    return mOffset;
}

void
FOdysseyPainterEditorAnimationTImelinePosition::SetPadding(float iPadding)
{
    mPadding = iPadding;
}

float
FOdysseyPainterEditorAnimationTImelinePosition::GetPadding() const
{
    return mPadding;
}

bool
FOdysseyPainterEditorAnimationTImelinePosition::HasMinZoom() const
{
    return mHasMinZoom;
}

void
FOdysseyPainterEditorAnimationTImelinePosition::HasMinZoom(bool iHasMinZoom)
{
    mHasMinZoom = iHasMinZoom;
}

bool
FOdysseyPainterEditorAnimationTImelinePosition::HasMaxZoom() const
{
    return mHasMaxZoom;
}

void
FOdysseyPainterEditorAnimationTImelinePosition::HasMaxZoom(bool iHasMaxZoom)
{
    mHasMaxZoom = iHasMaxZoom;
}

void
FOdysseyPainterEditorAnimationTImelinePosition::Reset()
{
    *this = FOdysseyPainterEditorAnimationTImelinePosition();
}
