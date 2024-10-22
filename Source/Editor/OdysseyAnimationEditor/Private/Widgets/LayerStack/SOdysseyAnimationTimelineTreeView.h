// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SCompoundWidget.h"

class UOdysseyAnimationLayerStack;
class UOdysseyAnimationPlayer;
class FOdysseyAnimationTimelineShortcuts;
class FOdysseyAnimationEditorTimelineCellSelection;
class FOdysseyAnimationEditorTimelinePosition;
class SOdysseyAnimationTimelineControl;
class ITableRow;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineTreeView
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineTreeView)
		: _HeaderHeight(25)
		, _ExternalScrollbar(nullptr)
        {}
        SLATE_ARGUMENT( UOdysseyAnimationLayerStack*, LayerStack )
		SLATE_ARGUMENT( UOdysseyAnimationPlayer*, Player )
        SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection>, TimelineCellSelection )
		SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
		SLATE_ARGUMENT( int, HeaderHeight )
		SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs, OnActivateOutOfPegs)
		SLATE_EVENT(FSimpleDelegate, OnInactivateOutOfPegs)
		SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked, OnIsOutOfPegsChecked)
		SLATE_ARGUMENT( TSharedPtr<SScrollBar>, ExternalScrollbar )
		SLATE_EVENT( FOnTableViewScrolled, OnTreeViewScrolled )
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs);

	TSharedPtr<SOdysseyAnimationLayerStackTreeView> GetTreeView() const;

private:
    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;

private:
	//Events
	TSharedRef<ITableRow> OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable);
    void OnGetChildren(UOdysseyLayer* iParent, TArray<UOdysseyLayer*>& oChildren) const;
	int GetCurrentFrame() const;

private:
	UOdysseyAnimationLayerStack* mLayerStack;
	UOdysseyAnimationPlayer* mPlayer;
    TSharedPtr<FOdysseyAnimationTimelineShortcuts> mTimelineShortcuts;
	TSharedPtr<FOdysseyAnimationEditorTimelineCellSelection> mTimelineCellSelection;
	TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
	TSharedPtr<SOdysseyAnimationTimelineControl> mTimelineControl;
	TSharedPtr<SOdysseyAnimationLayerStackTreeView> mTreeView;
	SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs mOnActivateOutOfPegs;
	FSimpleDelegate mOnInactivateOutOfPegs;
	SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked mOnIsOutOfPegsChecked;
};
