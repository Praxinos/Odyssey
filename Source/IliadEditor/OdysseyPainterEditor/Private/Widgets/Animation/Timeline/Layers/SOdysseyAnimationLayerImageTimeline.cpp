// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerImageTimeline.h"
#include "OdysseyLayerCellImageStagger.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLighttable.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineOutOfPegs.h"
#include "Widgets/Animation/Timeline/Cells/SOdysseyAnimationCells.h"
#include "TimelineTools/OdysseyAnimationTimelineTool.h"
#include "OdysseyAnimationCellsDragDropOperation.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyPainterEditorAnimationProjectSettings.h"
#include "OdysseyStyle.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellsShortcuts.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageStaggerShortcuts.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineScrollBox.h"
#include "TimelineTools/OdysseyAnimationTimelineTools.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyLayerCellSelection.h"
#include "ScopedTransaction.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Layout/SEnableBox.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageTimeline::~SOdysseyAnimationLayerImageTimeline()
{
}

SOdysseyAnimationLayerImageTimeline::SOdysseyAnimationLayerImageTimeline()
    : mIsDraggingOver(false)
    , mDragState(kDrag_None)
    , mDragPosition(0)
{
}

void
SOdysseyAnimationLayerImageTimeline::Construct(
    const FArguments& iArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    UOdysseyAnimationLayer* iLayer
)
{
    ensure(iLayer);
    mLayer = iLayer;
    mCurrentFrame = iArgs._CurrentFrame;
    mTimelinePosition = iArgs._TimelinePosition;
    mOnActivateOutOfPegs = iArgs._OnActivateOutOfPegs;
    mOnInactivateOutOfPegs = iArgs._OnInactivateOutOfPegs;
    mOnIsOutOfPegsChecked = iArgs._OnIsOutOfPegsChecked;
    mOnTransactCurrentFrame = iArgs._OnTransactCurrentFrame;

    SOdysseyAnimationLayerTimeline::Construct(SOdysseyAnimationLayerTimeline::FArguments(), iOwnerTableView, iLayer);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::GenerateWidgetForRow( const FName& iRow, const FName& iColumn )
{
    TSharedPtr<SWidget> widget = SOdysseyLayerRowBase::GenerateWidgetForRow( iRow, iColumn );

    widget = SNew(SOdysseyAnimationTimelineScrollBox)
        .TimelinePosition(mTimelinePosition)
        + SOdysseyAnimationTimelineScrollBox::Slot()
        [
            widget.ToSharedRef()
        ];

    if (iRow == "Main")
    {
        widget = SNew(SOverlay)
            + SOverlay::Slot()
            [
                SNew(SEnableBox)
                .IsEnabled_Lambda(
                    [this]()
                    {
                        if (!mLayer)
                            return false;

                        if (!mLayer->IsEditable())
                            return false;

                        return true;
                    }
                )
                [
                    widget.ToSharedRef()
                ]
            ]
            + SOverlay::Slot()
            [
                SNew(SColorBlock)
                .Visibility(EVisibility::SelfHitTestInvisible)
                .Color_Lambda(
                    [this]()
                    {
                        if (!mLayer)
                            return FLinearColor(0, 0, 0, 0);

                        if (mLayer->IsActivatedRecursively())
                            return FLinearColor(0, 0, 0, 0);

                        return FLinearColor(0, 0, 0, 0.75f);
                    }
                )
            ];
    }

    return widget.ToSharedRef();
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::GenerateWidget( const FName& iRow, const FName& iColumn )
{
    ensure(iColumn == "Timeline");

    if (iRow == "Main")
    {
        return GenerateMainRowTimelineWidget();
    }
    if (iRow == "Lighttable")
    {
        return GenerateLighttableRowTimelineWidget();
    }
    if (iRow == "OutOfPegs")
    {
        return GenerateOutOfPegsRowTimelineWidget();
    }

    return SOdysseyAnimationLayerTimeline::GenerateWidget( iRow, iColumn );
}

float
SOdysseyAnimationLayerImageTimeline::GetRowHeight(FName iRow) const
{
    if (iRow == "Main")
    {
        int height = GetLayer()->GetRowHeight("Main");

        if (GetLayer()->IsRowVisible("Blend"))
        {
            height += GetLayer()->GetRowHeight("Blend");
            height += GetLayer()->GetRowPadding("Blend").Bottom + GetLayer()->GetRowPadding("Blend").Top;
        }
        return height;
    }
    if (iRow == "Blend")
    {
        return 0;
    }
    return SOdysseyAnimationLayerTimeline::GetRowHeight(iRow);
}

EVisibility
SOdysseyAnimationLayerImageTimeline::GetRowVisibility(FName iRow) const
{
    if (iRow == "Blend")
    {
        return EVisibility::Collapsed;
    }

    return SOdysseyAnimationLayerTimeline::GetRowVisibility(iRow);
}

FMargin
SOdysseyAnimationLayerImageTimeline::GetRowPadding(FName iRow) const
{
    if (iRow == "Blend")
    {
        return FMargin(0);
    }

    return SOdysseyAnimationLayerTimeline::GetRowPadding(iRow);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::GenerateMainRowTimelineWidget()
{
    return SNew(SOdysseyAnimationCells, mLayer)
        .Cells_Lambda(
            [this]() -> TArray<UOdysseyLayerCell*>
            {
                return mLayer->GetCells();
            }
        )
        .CurrentFrame(mCurrentFrame)
        .OnTransactCurrentFrame(mOnTransactCurrentFrame)
        .ContextMenuExtender(CreateCellsContextMenuExtender())
        .TimelinePosition(mTimelinePosition)
        .IsEnabled_Lambda([this](){ return !mLayer->IsLockedRecursively();})
        .OnCreateCellWidget(this, &SOdysseyAnimationLayerImageTimeline::OnGenerateCellWidget)
        .ShowHandles(this, &SOdysseyAnimationLayerImageTimeline::GetShowCellsHandles);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::GenerateLighttableRowTimelineWidget()
{
    return SNew(SOdysseyAnimationTimelineLighttable, mLayer)
        .TimelinePosition(mTimelinePosition)
        .CurrentFrame(mCurrentFrame);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::GenerateOutOfPegsRowTimelineWidget()
{
    return SNew(SOdysseyAnimationTimelineOutOfPegs, mLayer)
        .CurrentFrame(mCurrentFrame)
        .TimelinePosition(mTimelinePosition)
        .OnActivateOutOfPegs(mOnActivateOutOfPegs)
        .OnInactivateOutOfPegs(mOnInactivateOutOfPegs)
        .OnIsOutOfPegsChecked(mOnIsOutOfPegsChecked);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::Layer,
        mLayer
    };

    mTool = FOdysseyAnimationTimelineTools::Get().CreateTool(mTimelinePosition.ToSharedRef(), mLayer->GetLayerStack()->GetCellSelection());
    if (!mTool)
        return FReply::Unhandled();

    FReply reply = mTool->OnMouseButtonDown(params);
    if (reply.IsEventHandled())
        return reply;

    if (iEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        int frame = (int)MousePositionToFrame(iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition()).X);
        if (frame < 0)
            return FReply::Unhandled();

        //On Right click, select cell if none are selected yet
        UOdysseyLayerCell* cell = mLayer->GetCellAtFrame(frame);
        if (!cell)
            return FReply::Unhandled();

        const TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
        if (selectedCells.IsEmpty() || !selectedCells.Contains(cell))
            mLayer->GetLayerStack()->GetCellSelection()->SetSelectedCells({cell});

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mTool)
        return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::Layer,
        mLayer
    };
    return mTool->OnMouseMove(params);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mTool)
        return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::Layer,
        mLayer
    };
    return mTool->OnDragDetected(params);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mTool)
        return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::Layer,
        mLayer
    };
    mTool->OnMouseButtonUp(params);
    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
    if (!mTool)
        return FReply::Unhandled();

    return mTool->OnKeyDown(iKeyEvent);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnKeyUp( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
    if (!mTool)
        return FReply::Unhandled();

    return mTool->OnKeyUp(iKeyEvent);
}

int32
SOdysseyAnimationLayerImageTimeline::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    // Draw a current frame
    LayerId = SOdysseyAnimationLayerTimeline::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
    ++LayerId;

    const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );

    const float height = AllottedGeometry.GetLocalSize().Y;
    const float width = AllottedGeometry.GetLocalSize().X;

    if(mIsDraggingOver && mDragState != kDrag_None)
    {
        //Dragging Zone
        FLinearColor lineColor(0.2f, 0.2f, 1.f);
        float dragPos = FrameToMousePosition(mDragPosition);

        //Dragging Bar
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( FVector2D(3.f, height), FSlateLayoutTransform( 1.0, TransformPoint( 1.0, FVector2D(dragPos, 0.f) ) ) ),
            GenericBrush,
            ESlateDrawEffect::None,
            lineColor
        );

        if (mDragState == kDrag_Copy)
        {
            int plusSize = 5.f;

            //Plus
            FSlateDrawElement::MakeBox(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry( FVector2D( 3 * plusSize, plusSize), FSlateLayoutTransform( 1.0, TransformPoint( 1.0, FVector2D(dragPos + 5.f,  5.f + plusSize) ) ) ),
                GenericBrush,
                ESlateDrawEffect::None,
                lineColor
            );

            //Plus
            FSlateDrawElement::MakeBox(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry( FVector2D(plusSize, 3 * plusSize), FSlateLayoutTransform( 1.0, TransformPoint( 1.0, FVector2D(dragPos + 5.f + plusSize,  5.f) ) ) ),
                GenericBrush,
                ESlateDrawEffect::None,
                lineColor
            );
        }
    }

    return LayerId;
}

void
SOdysseyAnimationLayerImageTimeline::OnDragEnter(const FGeometry& iGeometry, const FDragDropEvent& iEvent)
{
    TSharedPtr<FOdysseyAnimationCellsDragDropOperation> operation = iEvent.GetOperationAs<FOdysseyAnimationCellsDragDropOperation>();
    if (!operation)
        return;

    if (!operation->GetData().CanPaste(mLayer))
        return;

    mIsDraggingOver = true;
}

void
SOdysseyAnimationLayerImageTimeline::OnDragLeave(const FDragDropEvent& iEvent)
{
    TSharedPtr<FOdysseyAnimationCellsDragDropOperation> operation = iEvent.GetOperationAs<FOdysseyAnimationCellsDragDropOperation>();
    if (!operation)
        return;

    if (!operation->GetData().CanPaste(mLayer))
        return;

    mIsDraggingOver = false;
}

FReply
SOdysseyAnimationLayerImageTimeline::OnDragOver(const FGeometry& iGeometry, const FDragDropEvent& iEvent)
{
    TSharedPtr<FOdysseyAnimationCellsDragDropOperation> operation = iEvent.GetOperationAs<FOdysseyAnimationCellsDragDropOperation>();
    if (!operation)
        return FReply::Unhandled();

    if (!operation->GetData().CanPaste(mLayer))
        return FReply::Unhandled();

    float posX = iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition()).X;
    float frame = MousePositionToFrame(posX);

    UOdysseyLayerCell* cell = mLayer->GetCellAtFrame((int)frame);
    if (!cell)
        return FReply::Unhandled();

    int cellStartFrame = cell->GetFrameRange().GetLowerBoundValue();
    float position = (frame - cellStartFrame) / cell->GetExposure();
    if (position < 0.5f)
    {
        mDragPosition = cellStartFrame;
    }
    else
    {
        mDragPosition = cellStartFrame + cell->GetExposure();
    }

    /*
    if (FSlateApplication::Get().GetModifierKeys().IsControlDown())
        mDragState = kDrag_Copy;
    else
        mDragState = kDrag_Move;
    */
   mDragState = kDrag_Copy; //For now we can only copy cells, we will be able to move them when layers will have holes

    //Check if the copy or move is actually allowed
    UOdysseyAnimationLayer* layer = operation->GetLayer();
    if (layer == mLayer)
    {
        const TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
        UOdysseyLayerCell* nextCell = layer->GetCellAtFrame(mDragPosition);
        UOdysseyLayerCell* previousCell = layer->GetCellAtFrame(mDragPosition - 1);

        if (selectedCells.Contains(nextCell) && selectedCells.Contains(previousCell))
        {
            //Trying to copy cells inside the current layer selection
            //This is not allowed
            mDragState = kDrag_None;
            return FReply::Handled();
        }
    }

    return FReply::Handled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iEvent)
{
    TSharedPtr<FOdysseyAnimationCellsDragDropOperation> operation = iEvent.GetOperationAs<FOdysseyAnimationCellsDragDropOperation>();
    if (!operation)
        return FReply::Unhandled();

    if (mDragState == kDrag_None)
        return FReply::Unhandled();

    if (!operation->GetData().CanPaste(mLayer))
        return FReply::Unhandled();

    if (mDragState == kDrag_Copy)
    {
    #ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.dnd-copy", "Copy Cells"));
    #endif
        operation->GetData().Paste(mLayer, mDragPosition);
    }
    mIsDraggingOver = false;
    return FReply::Handled();
}

TSharedPtr<FExtender>
SOdysseyAnimationLayerImageTimeline::CreateCellsContextMenuExtender()
{
    return nullptr;
}

EVisibility
SOdysseyAnimationLayerImageTimeline::GetLighttableVisibility() const
{
    return mLayer->GetLighttable().bIsActivated && mLayer->ShouldDisplayOptions() ? EVisibility::Visible : EVisibility::Collapsed;
}

bool
SOdysseyAnimationLayerImageTimeline::GetShowCellsHandles() const
{
    return mLayer->ShouldDisplayOptions();
}

UOdysseyAnimationLayer*
SOdysseyAnimationLayerImageTimeline::GetLayer() const
{
    return mLayer;
}

float
SOdysseyAnimationLayerImageTimeline::MousePositionToFrame(float iX) const
{
    return (iX - mTimelinePosition->GetPadding() + mTimelinePosition->GetOffset() * mTimelinePosition->GetFrameSize()) / mTimelinePosition->GetFrameSize();
}

float
SOdysseyAnimationLayerImageTimeline::FrameToMousePosition(float iFrame) const
{
    return iFrame * mTimelinePosition->GetFrameSize() + mTimelinePosition->GetPadding() - mTimelinePosition->GetOffset() * mTimelinePosition->GetFrameSize();
}

#undef LOCTEXT_NAMESPACE
