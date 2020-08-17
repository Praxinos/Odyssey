// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "FOdysseyTimelineFrameDragDropOperation.h"

#define LOCTEXT_NAMESPACE "OdysseyTimeline"

FOdysseyTimelineFrameDragDropOperation::FOdysseyTimelineFrameDragDropOperation(TSharedPtr<SOdysseyTimelineFrame> iFrame, TSharedPtr<SOdysseyTimelineFrameList> iFrameList)
	: mFrame(iFrame)
	, mFrameList(iFrameList)
    , mTransaction(LOCTEXT("MovedFramesInTimeline", "Reorder key frames"))
{
	Construct();
	
	TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> metadata = iFrame->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
	if (!metadata)
		return;

	mFrameList->SetFrameVisibility(metadata->Index(), EVisibility::Collapsed);
}

TSharedPtr<SWidget>
FOdysseyTimelineFrameDragDropOperation::GetDefaultDecorator() const
{
    return mFrame;
}

void
FOdysseyTimelineFrameDragDropOperation::OnDragged(const class FDragDropEvent& DragDropEvent)
{
	if (CursorDecoratorWindow.IsValid())
	{
		CursorDecoratorWindow->MoveWindowTo(DragDropEvent.GetScreenSpacePosition());
	}
}

void
FOdysseyTimelineFrameDragDropOperation::Construct()
{
	MouseCursor = EMouseCursor::GrabHandClosed;
	FDragDropOperation::Construct();
}

void
FOdysseyTimelineFrameDragDropOperation::OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent)
{
	if (!bDropWasHandled)
	{
		TSharedPtr<FOdysseyTimelineFrameListFrameMetaData> metadata = mFrame->GetMetaData<FOdysseyTimelineFrameListFrameMetaData>();
		if (!metadata)
			return;

		mFrameList->SetFrameVisibility(metadata->Index(), EVisibility::Visible);
		// Add us back to our source, the drop fizzled
		mTransaction.Cancel();
	}
}

void
FOdysseyTimelineFrameDragDropOperation::SetCanDropHere(bool bCanDropHere)
{
    MouseCursor = bCanDropHere ? EMouseCursor::TextEditBeam : EMouseCursor::SlashedCircle;
}

TSharedPtr<SOdysseyTimelineFrame>&
FOdysseyTimelineFrameDragDropOperation::Frame()
{
	return mFrame;
}

#undef LOCTEXT_NAMESPACE