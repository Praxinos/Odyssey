// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Input/DragAndDrop.h"
#include "SOdysseyTimelineFrame.h"
#include "SOdysseyTimelineFrameList.h"

class FOdysseyTimelineFrameDragDropOperation : public FDragDropOperation
{
public:
	FOdysseyTimelineFrameDragDropOperation(TSharedPtr<SOdysseyTimelineFrame> iFrame, TSharedPtr<SOdysseyTimelineFrameList> iFrameList);

public:
	DRAG_DROP_OPERATOR_TYPE(FOdysseyTimelineFrameDragDropOperation, FDragDropOperation)

	// FDragDropOperation interface
	virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;
	virtual void OnDragged(const class FDragDropEvent& DragDropEvent) override;
	virtual void Construct() override;
	virtual void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override;
	// End of FDragDropOperation interface²

	void SetCanDropHere(bool bCanDropHere);

	TSharedPtr<SOdysseyTimelineFrame>& Frame();

private:
    TSharedPtr<SOdysseyTimelineFrame> mFrame;
	TSharedPtr<SOdysseyTimelineFrameList> mFrameList;
	FScopedTransaction mTransaction; //Don't know why we need this
};
