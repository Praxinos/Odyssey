// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;
class UOdysseyLayerStack;
class UOdysseyAnimationLayer;
class FOdysseyAnimationTimelineTool;
class FOdysseyAnimationTimelineSelectionTool;
class FOdysseyAnimationTimelineMoveTool;
class FOdysseyAnimationTimelineCutTool;
class UOdysseyAnimationCell;

enum class EOdysseyTimelineTool
{
    None,
    Selection,
    Move,
    Cut
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
    EOdysseyTimelineTool GetCurrentTool() const;
    void SetSelectedTool(EOdysseyTimelineTool iTool);

    void ZoomIn();
    void ZoomOut();

	void SetZoom(float iZoom);
	void SetOffset(float iOffset);
    //void SetSelectedFrames(const FInt32Range& iSelectedFrames);
    void SelectCell(UOdysseyAnimationCell* iCell, bool iSetAsCursor = false);
    void SetSelectedCells(const TArray<UOdysseyAnimationCell*>& iCells);

	static float GetBaseFrameSize();
	float GetFrameWidth() const;
	float GetZoom() const;
	float GetOffset() const;
	float GetPadding() const;
    //FInt32Range GetSelectedFrames() const;
    //FInt32Range GetSelectableFrames() const;
    const TArray<UOdysseyAnimationCell*>& GetSelectedCells() const;
    UOdysseyAnimationCell* GetCellSelectionCursor() const;
    void CleanSelectedCells();
    void CleanCellSelectionCursor();

    float MousePositionToFrame(float iPos) const;
	float FrameToMousePosition(float iFrame) const;

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
    //FInt32Range mSelectedFrames;
    TArray<UOdysseyAnimationCell*> mSelectedCells;
    UOdysseyAnimationCell* mCellSelectionCursor;

    FSimpleMulticastDelegate mOnZoomChanged;
    FSimpleMulticastDelegate mOnOffsetChanged;

	UOdysseyAnimationLayer* mLayer;

    EOdysseyTimelineTool mSelectedTool;
    TSharedPtr<FOdysseyAnimationTimelineSelectionTool> mSelectionTool;
    TSharedPtr<FOdysseyAnimationTimelineMoveTool> mMoveTool;
    TSharedPtr<FOdysseyAnimationTimelineCutTool> mCutTool;
};