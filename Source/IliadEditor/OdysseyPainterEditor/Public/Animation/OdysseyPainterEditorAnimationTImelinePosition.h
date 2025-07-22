// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorAnimationTimelinePosition
{
public:
    FOdysseyPainterEditorAnimationTimelinePosition(); //default

public:
    void CanZoom(bool iCanZoom);
    bool CanZoom() const;

    void SetBaseFrameSize(float iFrameSize);
    float GetBaseFrameSize() const;
    float GetFrameSize() const;

    void ZoomIn();
    void ZoomOut();
    void SetZoom(float iZoom);
    void SetMinZoom(float iZoom);
    void SetMaxZoom(float iZoom);
    void SetZoomStep(float iZoom);
    float GetZoom() const;
    float GetMinZoom() const;
    float GetMaxZoom() const;
    float GetZoomStep() const;

    void SetOffset(float iOffset);
    float GetOffset() const;

    void SetPadding(float iPadding);
    float GetPadding() const;

    bool HasMinZoom() const;
    void HasMinZoom(bool iHasMinZoom);
    bool HasMaxZoom() const;
    void HasMaxZoom(bool iHasMaxZoom);

    void Reset();

private:
    float mBaseFrameSize;
    float mOffset;
    float mZoom;
    float mMinZoom;
    float mMaxZoom;
    float mZoomStep;
    bool mCanZoom;
    float mPadding;
    bool mHasMinZoom;
    bool mHasMaxZoom;
};
