// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerImageTimeline.h"

class UOdysseyAnimationLayerImageRaster;
class FOdysseyAnimationTimelineCellImageRasterShortcuts;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageRasterTimeline
    : public SOdysseyAnimationLayerImageTimeline
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageRasterTimeline)
        {}
        SLATE_ATTRIBUTE(bool, DisplayOptions)
		SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
		SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection>, TimelineCellSelection )
		SLATE_EVENT(SOdysseyAnimationTimelineLightTableKey::FOnActivateOutOfPegs, OnActivateOutOfPegs)
		SLATE_EVENT(FSimpleDelegate, OnInactivateOutOfPegs)
		SLATE_EVENT(SOdysseyAnimationTimelineLightTableKey::FOnIsOutOfPegsChecked, OnIsOutOfPegsChecked)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    virtual ~SOdysseyAnimationLayerImageRasterTimeline();
    SOdysseyAnimationLayerImageRasterTimeline();
    void Construct(
        const FArguments& iArgs, 
        UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
    );

protected:
    //Context Menu
    virtual TSharedPtr<FExtender> ExtendContextMenu() override;

private:
    virtual TSharedRef<SWidget> OnGenerateCellWidget(UOdysseyAnimationCell* iCell) override;
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    bool GetShowCellContent() const;

private:
	TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
	TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection> mTimelineCellSelection;
    TSharedPtr<FOdysseyAnimationTimelineCellImageRasterShortcuts> mAnimationTimelineCellImageRasterShortcuts;
};