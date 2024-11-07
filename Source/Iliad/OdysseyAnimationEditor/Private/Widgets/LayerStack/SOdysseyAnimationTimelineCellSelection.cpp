// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyAnimationTimelineCellSelection.h"

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineCellSelection

/* SOdysseyAnimationTimelineCellSelection::SOdysseyAnimationTimelineCellSelection()
{
}

void
SOdysseyAnimationTimelineCellSelection::Construct(
    const FArguments& InArgs,
    FOdysseyAnimationEditorExtension* iExtension
)
{
    mExtension = iExtension;
    mCell = InArgs._Cell;
    mOnDragged = InArgs._OnDragged;

    ChildSlot
    [
        InArgs._Content.Widget
    ];
}

int32 SOdysseyAnimationTimelineCellSelection::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    // Draw a current frame
    LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
    ++LayerId;

    const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );

    const float height = AllottedGeometry.GetLocalSize().Y;
    const float width = AllottedGeometry.GetLocalSize().X;

    FLinearColor color = IsSelectionCursor() ? FLinearColor::Red : FLinearColor::Green;
    color.A = 0.2f;

    if(IsSelected())
    {
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( FVector2D(width, height), FSlateLayoutTransform() ),
            GenericBrush,
            ESlateDrawEffect::None,
            color
        );
    }

    return LayerId;
}

bool
SOdysseyAnimationTimelineCellSelection::IsSelected() const
{
    return mExtension->Timeline()->GetSelectedCells().Contains(mCell);
}

bool
SOdysseyAnimationTimelineCellSelection::IsSelectionCursor() const
{
    return mExtension->Timeline()->GetCellSelectionCursor() == mCell;
}

FReply
SOdysseyAnimationTimelineCellSelection::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (IsSelected() && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
        return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);

    return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}


FReply
SOdysseyAnimationTimelineCellSelection::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (mOnDragged.IsBound())
        return mOnDragged.Execute();

    return FReply::Unhandled();
} */
