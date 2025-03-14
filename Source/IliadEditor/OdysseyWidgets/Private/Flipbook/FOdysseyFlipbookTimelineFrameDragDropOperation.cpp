// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "FOdysseyFlipbookTimelineFrameDragDropOperation.h"

#define LOCTEXT_NAMESPACE "Widgets"

FOdysseyFlipbookTimelineFrameDragDropOperation::FOdysseyFlipbookTimelineFrameDragDropOperation(TSharedPtr<SOdysseyFlipbookTimelineFrame> iFrame, TSharedPtr<SOdysseyFlipbookTimelineFrameList> iFrameList)
    : mFrame(iFrame)
    , mFrameList(iFrameList)
    , mTransaction(LOCTEXT("timeline.drag-drop-operation.moved-frames-in-timeline", "Reorder key frames"))
{
    Construct();

    TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> metadata = iFrame->GetMetaData<FOdysseyFlipbookTimelineFrameListFrameMetaData>();
    if (!metadata)
        return;

    mFrameList->SetFrameVisibility(metadata->Index(), EVisibility::Collapsed);
}

TSharedPtr<SWidget>
FOdysseyFlipbookTimelineFrameDragDropOperation::GetDefaultDecorator() const
{
    return mFrame;
}

void
FOdysseyFlipbookTimelineFrameDragDropOperation::OnDragged(const class FDragDropEvent& DragDropEvent)
{
    if (CursorDecoratorWindow.IsValid())
    {
        CursorDecoratorWindow->MoveWindowTo(DragDropEvent.GetScreenSpacePosition());
    }
}

void
FOdysseyFlipbookTimelineFrameDragDropOperation::Construct()
{
    MouseCursor = EMouseCursor::GrabHandClosed;
    FDragDropOperation::Construct();
}

void
FOdysseyFlipbookTimelineFrameDragDropOperation::OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent)
{
    if (!bDropWasHandled)
    {
        TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> metadata = mFrame->GetMetaData<FOdysseyFlipbookTimelineFrameListFrameMetaData>();
        if (!metadata)
            return;

        mFrameList->SetFrameVisibility(metadata->Index(), EVisibility::Visible);
        // Add us back to our source, the drop fizzled
        mTransaction.Cancel();
    }
}

void
FOdysseyFlipbookTimelineFrameDragDropOperation::SetCanDropHere(bool bCanDropHere)
{
    MouseCursor = bCanDropHere ? EMouseCursor::TextEditBeam : EMouseCursor::SlashedCircle;
}

TSharedPtr<SOdysseyFlipbookTimelineFrame>&
FOdysseyFlipbookTimelineFrameDragDropOperation::Frame()
{
    return mFrame;
}

#undef LOCTEXT_NAMESPACE
