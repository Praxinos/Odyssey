// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorTimeline
{
public:
    FOdysseyAnimationEditorTimeline();

public:
    void ZoomIn();
    void ZoomOut();

	void SetZoom(float iZoom);
	void SetOffset(float iOffset);
    void SetSelectedFrames(const FInt32Range& iSelectedFrames);

	static float GetBaseFrameSize();
	float GetFrameWidth() const;
	float GetZoom() const;
	float GetOffset() const;
    FInt32Range GetSelectedFrames() const;

    int GetFrameIndexAtMousePosition(float iX) const;

public:
    //Events
    FSimpleMulticastDelegate& OnSelectedFramesChanged();
    FSimpleMulticastDelegate& OnOffsetChanged();
    FSimpleMulticastDelegate& OnZoomChanged();

private:
    float mZoom;
	float mOffset;
    FInt32Range mSelectedFrames;

    FSimpleMulticastDelegate mOnZoomChanged;
    FSimpleMulticastDelegate mOnOffsetChanged;
    FSimpleMulticastDelegate mOnSelectedFramesChanged;
};