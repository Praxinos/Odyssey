// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SOdysseyLayerRow.h"

class SOdysseyAnimationLayerStack;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerRow
    : public SOdysseyLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerRow)
        {}
		SLATE_ATTRIBUTE( int, CurrentFrame )
		SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
		SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection>, TimelineCellSelection )
    SLATE_END_ARGS()

public:
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        class UOdysseyAnimationLayer* iLayer
    );

protected:
	TAttribute<int> mCurrentFrame;
	TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
	TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection> mTimelineCellSelection;
};
