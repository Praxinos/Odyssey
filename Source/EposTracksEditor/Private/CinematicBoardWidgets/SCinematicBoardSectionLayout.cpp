// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionLayout.h"

#include "Brushes/SlateColorBrush.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionLayout"

//---

void
SCinematicBoardSectionLayout::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
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
            InArgs._Title.Widget
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            InArgs._Camera.Widget
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            InArgs._Thumbnails.Widget
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            InArgs._Planes.Widget
        ]
    ];
}

int32
SCinematicBoardSectionLayout::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
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
