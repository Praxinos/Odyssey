// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SOdysseyLayerRow.h"

class SOdysseyAnimationLayerStack;

/**
 * Implements a layer row widget
 */
class SOdysseyAnimationLayerRow
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
	virtual TSharedRef<SWidget> GenerateWidget( const FName& iRow, const FName& iColumn ) override;
	virtual TArray<TSharedPtr<SWidget>> GenerateMainRowHeaderOptionWidgets() override;

private:
	TSharedRef<SWidget> GenerateLightTableRowHeaderWidget();
	TSharedRef<SWidget> GenerateOutOfPegsRowHeaderWidget();
	void OnLightTableCheckStateChanged(ECheckBoxState iState);
	ECheckBoxState GetLightTableIsChecked() const;

protected:
	UOdysseyAnimationLayer* mLayer;
	TAttribute<int> mCurrentFrame;
	TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
	TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection> mTimelineCellSelection;
};
