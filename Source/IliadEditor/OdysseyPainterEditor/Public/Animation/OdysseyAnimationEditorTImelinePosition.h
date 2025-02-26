// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once

class ODYSSEYPAINTEREDITOR_API FOdysseyAnimationEditorTimelinePosition
{
public:
    FOdysseyAnimationEditorTimelinePosition(); //default

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
