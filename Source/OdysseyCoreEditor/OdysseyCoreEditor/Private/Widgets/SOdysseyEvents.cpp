// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/SOdysseyEvents.h"

void
SOdysseyEvents::Construct(const FArguments& iArgs)
{
    mOnMouseButtonDown = iArgs._OnMouseButtonDown;
    mOnMouseButtonUp = iArgs._OnMouseButtonUp;
    mOnMouseMove = iArgs._OnMouseMove;
    mOnMouseButtonDoubleClick = iArgs._OnMouseButtonDoubleClick;

    mOnDragDetected = iArgs._OnDragDetected;
    mOnDragEnter = iArgs._OnDragEnter;
    mOnDragOver = iArgs._OnDragOver;
    mOnDrop = iArgs._OnDrop;
    mOnDragLeave = iArgs._OnDragLeave;

    ChildSlot
    [
        iArgs._Content.Widget
    ];
}

FReply
SOdysseyEvents::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (!mOnMouseButtonDown.IsBound())
        return SCompoundWidget::OnMouseButtonDown(MyGeometry, MouseEvent);

    return mOnMouseButtonDown.Execute(MyGeometry, MouseEvent);
}

FReply
SOdysseyEvents::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (!mOnMouseMove.IsBound())
        return SCompoundWidget::OnMouseMove(MyGeometry, MouseEvent);

    return mOnMouseMove.Execute(MyGeometry, MouseEvent);
}

FReply
SOdysseyEvents::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (!mOnMouseButtonUp.IsBound())
        return SCompoundWidget::OnMouseButtonUp(MyGeometry, MouseEvent);

    return mOnMouseButtonUp.Execute(MyGeometry, MouseEvent);
}

FReply
SOdysseyEvents::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (!mOnMouseButtonDoubleClick.IsBound())
        return SCompoundWidget::OnMouseButtonDoubleClick(MyGeometry, MouseEvent);

    return mOnMouseButtonDoubleClick.Execute(MyGeometry, MouseEvent);
}

FReply
SOdysseyEvents::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (!mOnDragDetected.IsBound())
        return SCompoundWidget::OnDragDetected(MyGeometry, MouseEvent);

    return mOnDragDetected.Execute(MyGeometry, MouseEvent);
}

void
SOdysseyEvents::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    if (!mOnDragEnter.IsBound())
    {
        SCompoundWidget::OnDragEnter(MyGeometry, DragDropEvent);
        return;
    }

    mOnDragEnter.Execute(MyGeometry, DragDropEvent);
}

void
SOdysseyEvents::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
    if (!mOnDragLeave.IsBound())
    {
        SCompoundWidget::OnDragLeave(DragDropEvent);
        return;
    }

    mOnDragLeave.Execute(DragDropEvent);
}

FReply
SOdysseyEvents::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    if (!mOnDragOver.IsBound())
        return SCompoundWidget::OnDragOver(MyGeometry, DragDropEvent);

    return mOnDragOver.Execute(MyGeometry, DragDropEvent);
}

FReply
SOdysseyEvents::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    if (!mOnDrop.IsBound())
        return SCompoundWidget::OnDrop(MyGeometry, DragDropEvent);

    return mOnDrop.Execute(MyGeometry, DragDropEvent);
}
