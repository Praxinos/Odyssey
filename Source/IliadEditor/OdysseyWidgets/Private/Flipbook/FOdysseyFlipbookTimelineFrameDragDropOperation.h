// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Input/DragAndDrop.h"
#include "SOdysseyFlipbookTimelineFrame.h"
#include "SOdysseyFlipbookTimelineFrameList.h"
#include "ScopedTransaction.h"

class FOdysseyFlipbookTimelineFrameDragDropOperation : public FDragDropOperation
{
public:
    FOdysseyFlipbookTimelineFrameDragDropOperation(TSharedPtr<SOdysseyFlipbookTimelineFrame> iFrame, TSharedPtr<SOdysseyFlipbookTimelineFrameList> iFrameList);

public:
    DRAG_DROP_OPERATOR_TYPE(FOdysseyFlipbookTimelineFrameDragDropOperation, FDragDropOperation)

    // FDragDropOperation interface
    virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;
    virtual void OnDragged(const class FDragDropEvent& DragDropEvent) override;
    virtual void Construct() override;
    virtual void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override;
    // End of FDragDropOperation interface²

    void SetCanDropHere(bool bCanDropHere);

    TSharedPtr<SOdysseyFlipbookTimelineFrame>& Frame();

private:
    TSharedPtr<SOdysseyFlipbookTimelineFrame> mFrame;
    TSharedPtr<SOdysseyFlipbookTimelineFrameList> mFrameList;
    FScopedTransaction mTransaction; //Don't know why we need this
};
