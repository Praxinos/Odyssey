// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/SOdysseyLayerStackTreeView.h"

class UOdysseyAnimationLayerStack;
class FOdysseyAnimationTimelineShortcuts;
class FOdysseyPainterEditorAnimationTimelinePosition;
class UOdysseyAnimationCell;
class ITableRow;
class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API SOdysseyAnimationTimelineTreeView
    : public SOdysseyLayerStackTreeView
{
public:
    DECLARE_DELEGATE_OneParam(FOnActivateOutOfPegs, UOdysseyAnimationCell*)
    DECLARE_DELEGATE_RetVal_OneParam(ECheckBoxState, FOnIsOutOfPegsChecked, UOdysseyAnimationCell*)
    DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int>)
    DECLARE_DELEGATE_OneParam(FOnCurrentFrameChanged, int /*iFrame*/)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineTreeView)
        : _CurrentFrame(0)
        , _ExternalScrollbar(nullptr)
        {}
        SLATE_ARGUMENT( UOdysseyAnimationLayerStack*, LayerStack )
        SLATE_ATTRIBUTE( int, CurrentFrame )
        SLATE_ARGUMENT( TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition )
        SLATE_ARGUMENT( TSharedPtr<SScrollBar>, ExternalScrollbar )
        SLATE_EVENT(FOnActivateOutOfPegs, OnActivateOutOfPegs)
        SLATE_EVENT(FSimpleDelegate, OnInactivateOutOfPegs)
        SLATE_EVENT(FOnIsOutOfPegsChecked, OnIsOutOfPegsChecked)
        SLATE_EVENT( FOnTableViewScrolled, OnTreeViewScrolled )
        SLATE_EVENT(FSimpleDelegate, OnScrubStart)
        SLATE_EVENT(FSimpleDelegate, OnScrubEnd)
        SLATE_EVENT(FOnTransactCurrentFrame, OnTransactCurrentFrame)
        SLATE_EVENT(FOnCurrentFrameChanged, OnCurrentFrameChanged)
        SLATE_EVENT(FOnCurrentFrameChanged, OnCurrentFrameCommited)
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
    TAttribute<int> mCurrentFrame;
    UOdysseyAnimationLayerStack* mLayerStack;
    TSharedPtr<FOdysseyAnimationTimelineShortcuts> mTimelineShortcuts;
    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
    FOnActivateOutOfPegs mOnActivateOutOfPegs;
    FSimpleDelegate mOnInactivateOutOfPegs;
    FOnIsOutOfPegsChecked mOnIsOutOfPegsChecked;
};
