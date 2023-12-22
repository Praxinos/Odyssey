// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;
class UOdysseyLayerStack;
class FOdysseyAnimationCellsContainer;
class FOdysseyAnimationTimelineTool;
class FOdysseyAnimationTimelineSelectionTool;
class FOdysseyAnimationTimelineMoveTool;

enum class EOdysseyTimelineTool
{
    None,
    Selection,
    Move
};
class FOdysseyAnimationEditorTimeline
{
public:

public:
    ~FOdysseyAnimationEditorTimeline();
    FOdysseyAnimationEditorTimeline(FOdysseyAnimationEditorExtension* iExtension);

    void  Initialize();
    void  Finalize();

public:
    TSharedPtr<FOdysseyAnimationTimelineTool> GetTool() const;
    EOdysseyTimelineTool GetSelectedTool() const;
    void SetSelectedTool(EOdysseyTimelineTool iTool);

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
    FInt32Range GetSelectableFrames() const;

    int GetFrameIndexAtMousePosition(float iX) const;

public:
    //Events
    FSimpleMulticastDelegate& OnOffsetChanged();
    FSimpleMulticastDelegate& OnZoomChanged();

private:
    void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);
    void OnCellsChanged();
    void BindCurrentLayerChanged();
    void UnbindCurrentLayerChanged();
    void BindOnCellsChanged();
    void UnbindOnCellsChanged();

private:
    FOdysseyAnimationEditorExtension* mExtension;

    float mZoom;
	float mOffset;
    FInt32Range mSelectedFrames;

    FSimpleMulticastDelegate mOnZoomChanged;
    FSimpleMulticastDelegate mOnOffsetChanged;

    TWeakPtr<FOdysseyAnimationCellsContainer> mCellsContainer; 

    EOdysseyTimelineTool mSelectedTool;
    TSharedPtr<FOdysseyAnimationTimelineSelectionTool> mSelectionTool;
    TSharedPtr<FOdysseyAnimationTimelineMoveTool> mMoveTool;
};