// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/SOdysseyLayerStackTreeView.h"

class UOdysseyAnimationLayerStack;
class UOdysseyAnimationPlayer;
class FOdysseyAnimationTimelineShortcuts;
class FOdysseyAnimationEditorTimelinePosition;
class SOdysseyAnimationTimelineControl;
class UOdysseyAnimationCell;
class ITableRow;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineTreeView
    : public SOdysseyLayerStackTreeView
{
public:
	DECLARE_DELEGATE_OneParam(FOnActivateOutOfPegs, UOdysseyAnimationCell*)
	DECLARE_DELEGATE_RetVal_OneParam(ECheckBoxState, FOnIsOutOfPegsChecked, UOdysseyAnimationCell*)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineTreeView)
		: _Player(nullptr)
		, _ExternalScrollbar(nullptr)
        {}
        SLATE_ARGUMENT( UOdysseyAnimationLayerStack*, LayerStack )
		SLATE_ARGUMENT( UOdysseyAnimationPlayer*, Player )
		SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
		SLATE_EVENT(FOnActivateOutOfPegs, OnActivateOutOfPegs)
		SLATE_EVENT(FSimpleDelegate, OnInactivateOutOfPegs)
		SLATE_EVENT(FOnIsOutOfPegsChecked, OnIsOutOfPegsChecked)
		SLATE_ARGUMENT( TSharedPtr<SScrollBar>, ExternalScrollbar )
		SLATE_EVENT( FOnTableViewScrolled, OnTreeViewScrolled )
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs);

private:
    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;
	virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

private:
	//Events
	TSharedRef<ITableRow> OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable);
    void OnGetChildren(UOdysseyLayer* iParent, TArray<UOdysseyLayer*>& oChildren) const;

private:
	UOdysseyAnimationLayerStack* mLayerStack;
	UOdysseyAnimationPlayer* mPlayer;
    TSharedPtr<FOdysseyAnimationTimelineShortcuts> mTimelineShortcuts;
	TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
	FOnActivateOutOfPegs mOnActivateOutOfPegs;
	FSimpleDelegate mOnInactivateOutOfPegs;
	FOnIsOutOfPegsChecked mOnIsOutOfPegsChecked;
};
