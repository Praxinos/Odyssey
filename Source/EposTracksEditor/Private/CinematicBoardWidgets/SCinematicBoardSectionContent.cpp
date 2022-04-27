// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "CinematicBoardWidgets/SCinematicBoardSectionContent.h"

#include "Brushes/SlateColorBrush.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardWidgets/SCinematicBoardSectionCamera.h"
#include "CinematicBoardWidgets/SCinematicBoardSectionNotes.h"
#include "CinematicBoardWidgets/SCinematicBoardSectionPlanes.h"
#include "CinematicBoardWidgets/SCinematicBoardSectionThumbnails.h"
#include "CinematicBoardWidgets/SCinematicBoardSectionTitle.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionContent"

//---

void
SCinematicBoardSectionContent::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    ChildSlot
    // Because in ...\Sequencer\Private\DisplayNodes\SequencerTrackNode.cpp - FSequencerTrackNode::GetNodeHeight(), there is an additional padding 2 * SequencerNodeConstants::CommonPadding for the 'mother' section
    // and VAlign can't be used, because f.e., one shot section with planes and one board section without, the content of the board section won't be at the same level as in shot section
    //.Padding( 0.f, 4.f )
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew( mWidgetTitle, SCinematicBoardSectionTitle, iBoardSection )
            .OptionalWidgetsVisibility( this, &SCinematicBoardSectionContent::OptionalWidgetsVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionCamera, iBoardSection )
            .OptionalWidgetsVisibility( this, &SCinematicBoardSectionContent::OptionalWidgetsVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionThumbnails, iBoardSection )
            .OptionalWidgetsVisibility( this, &SCinematicBoardSectionContent::OptionalWidgetsVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionNotes, iBoardSection )
            .OptionalWidgetsVisibility( this, &SCinematicBoardSectionContent::OptionalWidgetsVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionPlanes, iBoardSection )
            .OptionalWidgetsVisibility( this, &SCinematicBoardSectionContent::OptionalWidgetsVisibility )
        ]
    ];
}

int32
SCinematicBoardSectionContent::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

void
SCinematicBoardSectionContent::EnterRename()
{
    mWidgetTitle->EnterRename();
}

EVisibility
SCinematicBoardSectionContent::OptionalWidgetsVisibility() const
{
    return mOptionalWidgetsVisibility;
}

void
SCinematicBoardSectionContent::OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    mOptionalWidgetsVisibility = EVisibility::Visible;
}

void
SCinematicBoardSectionContent::OnMouseLeave( const FPointerEvent& MouseEvent ) //override
{
    mOptionalWidgetsVisibility = EVisibility::Hidden;
}

//FReply
//SOdysseyTimelineFrame::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
//{
//  if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
//  {
//      return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
//  }
//
//  return FReply::Unhandled();
//}
//
//FReply
//SOdysseyTimelineFrame::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
//{
//  if (mOnDragDetected.IsBound())
//  {
//      return mOnDragDetected.Execute(iGeometry, iMouseEvent);
//  }
//  return FReply::Unhandled();
//}
//
//void
//SOdysseyTimelineFrame::OnDragEnter(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
//{
//  if (mOnDragEnter.IsBound())
//  {
//      mOnDragEnter.Execute(iGeometry, iDragDropEvent);
//  }
//}
//
//void
//SOdysseyTimelineFrame::OnDragLeave(const FDragDropEvent& iDragDropEvent)
//{
//  if (mOnDragLeave.IsBound())
//  {
//      mOnDragLeave.Execute(iDragDropEvent);
//  }
//}
//
//FReply
//SOdysseyTimelineFrame::OnDragOver(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
//{
//  if (mOnDragOver.IsBound())
//  {
//      return mOnDragOver.Execute(iGeometry, iDragDropEvent);
//  }
//  return FReply::Unhandled();
//}
//
//
//FReply
//SOdysseyTimelineFrame::OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
//{
//  if (mOnDrop.IsBound())
//  {
//      return mOnDrop.Execute(iGeometry, iDragDropEvent);
//  }
//  return FReply::Unhandled();
//}
//
//FReply
//SOdysseyTimelineFrame::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
//{
//  if (iMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && mOnGenerateContextMenu.IsBound())
//  {
//      return mOnGenerateContextMenu.Execute(iGeometry, iMouseEvent);
//  }
//  return FReply::Unhandled();
//}

#undef LOCTEXT_NAMESPACE
