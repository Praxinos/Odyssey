// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationLayerImageTimeline.h"

#include "Algo/Accumulate.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/GenericCommands.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Layout/WidgetPath.h"
#include "Misc/MessageDialog.h"
#include "ScopedTransaction.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SEnableBox.h"

#include "OdysseyLayerCellImageStagger.h"
#include "SOdysseyAnimationTimelineLighttable.h"
#include "SOdysseyAnimationTimelineOutOfPegs.h"
#include "SOdysseyAnimationTimelineCellNames.h"
#include "SOdysseyAnimationCells.h"
#include "TimelineTools/OdysseyAnimationTimelineTool.h"
#include "OdysseyAnimationCellsDragDropOperation.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyPainterEditorAnimationProjectSettings.h"
#include "OdysseyStyle.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellsShortcuts.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageStaggerShortcuts.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "SOdysseyAnimationTimelineScrollBox.h"
#include "TimelineTools/OdysseyAnimationTimelineTools.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyLayerCellSelection.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorMassModifierView.h"
#include "Widgets/SOdysseyEvents.h"
#include "OdysseyVectorGroupPaint.h"

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

    TSharedRef<SOdysseyEvents> eventWidget = SNew(SOdysseyEvents)
        .OnMouseButtonDown(this, &SOdysseyAnimationLayerImageTimeline::OnSubRowMouseButtonDown, iRow)
        .OnMouseMove(this, &SOdysseyAnimationLayerImageTimeline::OnSubRowMouseMove, iRow)
        .OnMouseButtonUp(this, &SOdysseyAnimationLayerImageTimeline::OnSubRowMouseButtonUp, iRow)
        .OnDragDetected(this, &SOdysseyAnimationLayerImageTimeline::OnSubRowDragDetected, iRow)
        .OnDragEnter(this, &SOdysseyAnimationLayerImageTimeline::OnSubRowDragEnter, iRow)
        .OnDragLeave(this, &SOdysseyAnimationLayerImageTimeline::OnSubRowDragLeave, iRow)
        .OnDragOver(this, &SOdysseyAnimationLayerImageTimeline::OnSubRowDragOver, iRow)
        .OnDrop(this, &SOdysseyAnimationLayerImageTimeline::OnSubRowDrop, iRow)
        [
            SNew(SOdysseyAnimationTimelineScrollBox)
            .TimelinePosition(mTimelinePosition)
            + SOdysseyAnimationTimelineScrollBox::Slot()
            [
                widget.ToSharedRef()
            ]
        ];

    if (mEventWidgets.Contains(iRow))
        mEventWidgets[iRow] = eventWidget;
    else
        mEventWidgets.Add(iRow, eventWidget);

    widget = SNew(SOverlay)
        + SOverlay::Slot()
        [
            SNew(SEnableBox)
            .IsEnabled(this, &SOdysseyAnimationLayerImageTimeline::IsRowEnabled, iRow)
            [
                eventWidget
            ]
        ]
        + SOverlay::Slot()
        [
            SNew(SBorder)
            .Padding(0)
            .Visibility(this, &SOdysseyAnimationLayerImageTimeline::GetRowDisabledColorVisibility, iRow)
            .BorderImage(this, &SOdysseyAnimationLayerImageTimeline::GetRowDisabledColorValue, iRow)
        ];

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
    if (iRow == "CellNames")
    {
        return GenerateCellNamesRowTimelineWidget();
    }

    return SOdysseyAnimationLayerTimeline::GenerateWidget( iRow, iColumn );
}

bool
SOdysseyAnimationLayerImageTimeline::IsRowEnabled(FName iRow) const
{
    if (iRow == "Lighttable" || iRow == "OutOfPegs")
        return true;

    return mLayer && mLayer->IsEditable();
}

EVisibility
SOdysseyAnimationLayerImageTimeline::GetRowDisabledColorVisibility(FName iRow) const
{
    if (IsRowEnabled(iRow))
        return EVisibility::Collapsed;

    return EVisibility::SelfHitTestInvisible;
}

const FSlateBrush*
SOdysseyAnimationLayerImageTimeline::GetRowDisabledColorValue(FName iRow) const
{
    static const FSlateBrush* nobrush = new FSlateNoResource();

    if (IsRowEnabled(iRow))
        return nobrush;

    if( mLayer && !mLayer->IsActivatedRecursively() )
        return FOdysseyStyle::Get().GetBrush( "Animation.Timeline.DeactivatedOverlay" );

    if( mLayer && mLayer->IsLockedRecursively() )
        return FOdysseyStyle::Get().GetBrush( "Animation.Timeline.LockedOverlay" );

    return nobrush;
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

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::GenerateCellNamesRowTimelineWidget()
{
    return SNew(SOdysseyAnimationTimelineCellNames, mLayer)
        .TimelinePosition(mTimelinePosition)
        .CurrentFrame(mCurrentFrame);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnSubRowMouseButtonDown( const FGeometry& iGeometry, const FPointerEvent& iEvent, FName iRow )
{
    if (iRow == "Main")
    {
        return OnMainSubRowMouseButtonDown( iGeometry, iEvent );
    }
    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMainSubRowMouseButtonDown( const FGeometry& iGeometry, const FPointerEvent& iEvent )
{
    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        mEventWidgets["Main"].ToSharedRef(),
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

        return FReply::Handled().CaptureMouse( mEventWidgets["Main"].ToSharedRef() );
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnSubRowMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent, FName iRow)
{
    if (iRow == "Main")
    {
        return OnMainSubRowMouseMove( iGeometry, iEvent );
    }
    return FReply::Unhandled();
}


FReply
SOdysseyAnimationLayerImageTimeline::OnMainSubRowMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mTool)
        return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        mEventWidgets["Main"].ToSharedRef(),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::Layer,
        mLayer
    };
    return mTool->OnMouseMove(params);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnSubRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent, FName iRow)
{
    if (iRow == "Main")
    {
        return OnMainSubRowDragDetected( iGeometry, iEvent );
    }
    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMainSubRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mTool)
        return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        mEventWidgets["Main"].ToSharedRef(),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::Layer,
        mLayer
    };
    return mTool->OnDragDetected(params);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnSubRowMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent, FName iRow)
{
    if (iRow == "Main")
    {
        return OnMainSubRowMouseButtonUp( iGeometry, iEvent );
    }
    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMainSubRowMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mTool)
        return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        mEventWidgets["Main"].ToSharedRef(),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::Layer,
        mLayer
    };

    FReply reply = mTool->OnMouseButtonUp(params);
    if (reply.IsEventHandled())
        return reply;

    if (iEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        FFrameNumber frame = (int)MousePositionToFrame( iGeometry.AbsoluteToLocal( iEvent.GetScreenSpacePosition() ).X );

        //Open the context menu
        TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
        FMenuBuilder menuBuilder(true, commandList);

        BuildContextMenu(commandList, menuBuilder, frame);

        TSharedRef<SWidget> menuContents = menuBuilder.MakeWidget();
        FWidgetPath widgetPath = iEvent.GetEventPath() != nullptr ? *iEvent.GetEventPath() : FWidgetPath();
        FSlateApplication::Get().PushMenu(mEventWidgets["Main"].ToSharedRef(), widgetPath, menuContents, iEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
        return FReply::Handled();
    }
    return FReply::Unhandled().ReleaseMouseCapture();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnContextMenuMinusButtonClicked()
{
    mAnimationTimelineCellsShortcuts->Action_DecreaseCellExposure();
    return FReply::Handled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnContextMenuPlusButtonClicked()
{
    mAnimationTimelineCellsShortcuts->Action_IncreaseCellExposure();
    return FReply::Handled();
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

FCursorReply
SOdysseyAnimationLayerImageTimeline::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    static EOdysseyTimelineTool mode = EOdysseyTimelineTool::None;
    static FMouseCursor mouseCursor( EMouseCursor::Default );
    if( mode != FOdysseyAnimationTimelineTools::Get().GetCurrentTool() )
    {
        mode = FOdysseyAnimationTimelineTools::Get().GetCurrentTool();
        mouseCursor = EMouseCursor::Default;

        TSharedPtr<FOdysseyAnimationTimelineTool> tool = FOdysseyAnimationTimelineTools::Get().CreateTool( mTimelinePosition.ToSharedRef(), mLayer->GetLayerStack()->GetCellSelection() );
        if( tool )
            mouseCursor = tool->GetMouseCursor();
    }

    //---

    mouseCursor.UpdateCursor();

    return FCursorReply::Cursor( mouseCursor.GetMouseCursorNative() );
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
SOdysseyAnimationLayerImageTimeline::OnSubRowDragEnter(const FGeometry& iGeometry, const FDragDropEvent& iEvent, FName iRow)
{
    if (iRow == "Main")
        OnMainSubRowDragEnter( iGeometry, iEvent );
}

void
SOdysseyAnimationLayerImageTimeline::OnMainSubRowDragEnter(const FGeometry& iGeometry, const FDragDropEvent& iEvent)
{
    TSharedPtr<FOdysseyAnimationCellsDragDropOperation> operation = iEvent.GetOperationAs<FOdysseyAnimationCellsDragDropOperation>();
    if (!operation)
        return;

    if (!operation->GetData().CanPaste(mLayer))
        return;

    mIsDraggingOver = true;
}

void
SOdysseyAnimationLayerImageTimeline::OnSubRowDragLeave(const FDragDropEvent& iEvent, FName iRow)
{
    if (iRow == "Main")
        OnMainSubRowDragLeave( iEvent );
}

void
SOdysseyAnimationLayerImageTimeline::OnMainSubRowDragLeave(const FDragDropEvent& iEvent)
{
    TSharedPtr<FOdysseyAnimationCellsDragDropOperation> operation = iEvent.GetOperationAs<FOdysseyAnimationCellsDragDropOperation>();
    if (!operation)
        return;

    if (!operation->GetData().CanPaste(mLayer))
        return;

    mIsDraggingOver = false;
}

FReply
SOdysseyAnimationLayerImageTimeline::OnSubRowDragOver(const FGeometry& iGeometry, const FDragDropEvent& iEvent, FName iRow)
{
    if (iRow == "Main")
    {
        return OnMainSubRowDragOver( iGeometry, iEvent );
    }
    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMainSubRowDragOver(const FGeometry& iGeometry, const FDragDropEvent& iEvent)
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
SOdysseyAnimationLayerImageTimeline::OnSubRowDrop(const FGeometry& iGeometry, const FDragDropEvent& iEvent, FName iRow)
{
    if (iRow == "Main")
    {
        return OnMainSubRowDrop( iGeometry, iEvent );
    }
    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMainSubRowDrop(const FGeometry& iGeometry, const FDragDropEvent& iEvent)
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
        FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.dnd-copy", "Copy Cells"));

        operation->GetData().Paste(mLayer, mDragPosition);
    }
    mIsDraggingOver = false;
    return FReply::Handled();
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

void
SOdysseyAnimationLayerImageTimeline::BuildContextMenu(TSharedRef<FUICommandList> CommandList, FMenuBuilder& MenuBuilder, FFrameNumber iClickedFrame)
{
    mAnimationTimelineCellsShortcuts = MakeShared<FOdysseyAnimationTimelineCellsShortcuts>(mLayer->GetAnimation(), mCurrentFrame, mOnTransactCurrentFrame);
    mAnimationTimelineCellsShortcuts->MapActionsToCommandList(CommandList);

    mAnimationTimelineCellImageStaggerShortcuts = MakeShared<FOdysseyAnimationTimelineCellImageStaggerShortcuts>(mLayer->GetAnimation());
    mAnimationTimelineCellImageStaggerShortcuts->MapActionsToCommandList(CommandList);

    //TODO: harmonize with shortcuts ? see comment below
    auto IsReadOnly = [this]()
        {
            UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>( mLayer->GetLayerStack() );
            if( !layerStack )
                return true;

            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>( layerStack->GetCurrentLayer() );
            if( !layer )
                return true;

            if( !layer->IsEditable() )
                return true;

            TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
            if( selectedCells.IsEmpty() )
            {
                UOdysseyLayerCell* cell = layer->GetCellAtFrame( mCurrentFrame.Get() );
                if( !cell )
                    return true;
            }

            return false;
        };

    MenuBuilder.AddWidget(
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .Padding(FMargin(4, 0, 2, 0))
        [
            SNew(SButton)
            .OnClicked(this, &SOdysseyAnimationLayerImageTimeline::OnContextMenuMinusButtonClicked)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("-")))
                .Justification(ETextJustify::Center)
            ]
        ]
        +SHorizontalBox::Slot()
        .Padding(FMargin(2, 0, 4, 0))
        [
            SNew(SButton)
            .OnClicked(this, &SOdysseyAnimationLayerImageTimeline::OnContextMenuPlusButtonClicked)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("+")))
                .Justification(ETextJustify::Center)
            ]
        ],
        FText(),
        true,
        false
    );
    //MenuBuilder.EndSection();

    MenuBuilder.BeginSection("Selection", LOCTEXT("timeline-cells.context-menu.selection-section.name", "Selection"));
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().SelectAll);
    MenuBuilder.EndSection();

    MenuBuilder.BeginSection("Common", LOCTEXT("timeline-cells.context-menu.common-section.name", "Common"));
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Cut);
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Paste);
        MenuBuilder.AddSeparator("");
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
    MenuBuilder.EndSection();

    MenuBuilder.BeginSection("Cells", LOCTEXT("timeline-cells.context-menu.cells-section.name", "Cells"));
        MenuBuilder.AddSubMenu(
            LOCTEXT( "timeline-cells.context-menu.create-stagger-cell.name", "Stagger" ),
            LOCTEXT( "timeline-cells.context-menu.create-stagger-cell.tooltip", "Create stagger cell from selected cells" ),
            FNewMenuDelegate::CreateLambda( [this]( FMenuBuilder& ioMenuBuilder )
                                            {
                                                ioMenuBuilder.AddMenuEntry(FOdysseyPainterEditorAnimationCommands::Get().CreateStaggerCellLoop, NAME_None, LOCTEXT("timeline-cells.context-menu.stagger-cell-loop.name", "Loop"));
                                                ioMenuBuilder.AddMenuEntry(FOdysseyPainterEditorAnimationCommands::Get().CreateStaggerCellPingPong, NAME_None, LOCTEXT("timeline-cells.context-menu.stagger-cell-pingpong.name", "Ping-Pong"));
                                                ioMenuBuilder.AddMenuEntry(FOdysseyPainterEditorAnimationCommands::Get().CreateStaggerCellRandom, NAME_None, LOCTEXT("timeline-cells.context-menu.stagger-cell-random.name", "Random"));
                                            } ),
            false, // bInOpenSubMenuOnClick
            FSlateIcon(),
            true, // bInShouldCloseWindowAfterMenuSelection
            "CreateStagger"
        );
        MenuBuilder.AddMenuEntry(FOdysseyPainterEditorAnimationCommands::Get().ConvertToReferenceCells);
        MenuBuilder.AddMenuEntry(
            FOdysseyPainterEditorAnimationCommands::Get().SetCellExposure,
            NAME_None,
            LOCTEXT("timeline-cells.context-menu.set-selected-cells-exposure.name", "Set Exposure")
        );
        //TODO: add functions to shortcuts ? but it must be 2 distinct functions: 1 for shortcut with modal request and 1 for the popup menu here with a string/text as parameter
        // and the first one must call the second one
        MenuBuilder.AddEditableText(
            //LOCTEXT( "timeline-cells.context-menu.set-selected-cells-name.name", "" ),
            LOCTEXT( "timeline-cells.context-menu.set-selected-cells-name.name", "   Name" ),
            LOCTEXT( "timeline-cells.context-menu.set-selected-cells-name.tooltip", "Set the cell name" ),
            FSlateIcon(),
            TAttribute<FText>::CreateLambda( [this]() -> FText
                                             {
                                                 TArray<FString> selected_names;
                                                 const TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
                                                 if( selectedCells.IsEmpty() )
                                                 {
                                                     UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( mCurrentFrame.Get() );
                                                     return cell ? FText::FromString( cell->GetName( ECellNameIfEmpty::None ) ) : FText::GetEmpty();
                                                 }
                                                 for( const UOdysseyLayerCell* selectedcell : selectedCells )
                                                     selected_names.AddUnique( selectedcell->GetName( ECellNameIfEmpty::None ) );

                                                 if( selected_names.IsEmpty() )
                                                     return FText::GetEmpty();

                                                 if( selected_names.Num() != 1 )
                                                     return FText::FromString( TEXT( "*" ) );

                                                 return FText::FromString( selected_names[0] );
                                             } ),
            FOnTextCommitted::CreateLambda( [this]( const FText& iNewText, ETextCommit::Type iCommitType )
                                            {
                                                if( iCommitType != ETextCommit::OnEnter )
                                                    return;

                                                if( iNewText.ToString() == TEXT( "*" ) )
                                                    return;

                                                const TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
                                                if( selectedCells.IsEmpty() )
                                                {
                                                    UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( mCurrentFrame.Get() );
                                                    cell->SetName( iNewText.ToString() );
                                                    return;
                                                }
                                                for( UOdysseyLayerCell* selectedcell : selectedCells )
                                                    selectedcell->SetName( iNewText.ToString() );
                                            } ),
            FOnTextChanged(),
            IsReadOnly()
        );
        MenuBuilder.AddSubMenu(
            LOCTEXT("timeline-cells.context-menu.cell-mark.name", "Mark"),
            LOCTEXT("timeline-cells.context-menu.cell-mark.tooltip", "Set a mark on the selected cells"),
            FNewMenuDelegate::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::BuildCellsMarksSubMenu, iClickedFrame)
        );
        MenuBuilder.AddMenuEntry(
              FOdysseyPainterEditorAnimationCommands::Get().ReverseSelectedCells
            , NAME_None
            , LOCTEXT("timeline-cells.context-menu.reverse-selected-cells.name", "Reverse Selected Cells")
            , LOCTEXT("timeline-cells.context-menu.reverse-selected-cells.tooltip", "Reverse the order of the selected cells")
            , FSlateIcon()
        );
    MenuBuilder.EndSection();

    if( mLayer->GetClass() == UOdysseyAnimationLayerImageVector::StaticClass() )
    {
        MenuBuilder.BeginSection("More", LOCTEXT("timeline-cells.context-menu.mass-modifier.name", "Mass Modifier"));
        MenuBuilder.AddMenuEntry(
              LOCTEXT("timeline-cells.context-menu.mass-modifier.name", "Mass Modifier")
            , LOCTEXT("timeline-cells.context-menu.mass-modifier.tooltip", "Mass Modifier")
            , FSlateIcon()
            , FUIAction( FExecuteAction::CreateSP( this, &SOdysseyAnimationLayerImageTimeline::MassModifier ) ) );

        MenuBuilder.EndSection();
    }
}

FReply
SOdysseyAnimationLayerImageTimeline::MassModifierAcceptProperties( TSharedRef<SOdysseyPainterEditorVectorMassModifierView> iMassModifierView )
{
    TSharedPtr<SWindow> topWindow;

    iMassModifierView.Get().UndoPreview();
    iMassModifierView.Get().ValidateProperties();

    topWindow = FSlateApplicationBase::Get().GetActiveTopLevelWindow();

    FSlateApplicationBase::Get().RequestDestroyWindow( topWindow.ToSharedRef() );

    return FReply::Handled();
}

void
SOdysseyAnimationLayerImageTimeline::MassModifier()
{
    UOdysseyAnimationLayerImageVector* layerImageVector = Cast<UOdysseyAnimationLayerImageVector>(mLayer->GetLayerStack()->GetCurrentLayer());
    TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    TArray<FOdysseyVectorGroupPaint*> vectorSceneArray;
    FOdysseyVectorGroupPaint* previewScene;
    FOdysseyVectorGroupPaint* previewSceneCopy = nullptr;

    vectorSceneArray.Reserve( selectedCells.Num() );

    if (selectedCells.IsEmpty())
        return;

    // prevent multiple instances of the mass modifier
    //if( bMassModifierWindowRunning == false )
    {
        for( UOdysseyLayerCell* cell : selectedCells )
        {
            UOdysseyAnimationCellImageVector* animationCell = Cast<UOdysseyAnimationCellImageVector>(cell);

            if( animationCell )
            {
                vectorSceneArray.Push( animationCell->GetVectorCell()->GetScene() );
            }
        }

        UOdysseyLayerCell* cell = mLayer->GetCellAtFrame(mCurrentFrame.Get());
        UOdysseyAnimationCellImageVector* imageVectorCell = Cast<UOdysseyAnimationCellImageVector>(cell);

        previewScene = imageVectorCell ? imageVectorCell->GetVectorCell()->GetScene() : nullptr;

        TSharedRef<SOdysseyPainterEditorVectorMassModifierView> massModifierView = SNew(SOdysseyPainterEditorVectorMassModifierView)
                                                                                   .VectorLayer( layerImageVector->GetVectorLayer() )
                                                                                   .SceneArray(vectorSceneArray)
                                                                                   .PreviewScene( previewScene );

        TSharedRef<SWindow> ObjectWindow = SNew(SWindow)
        .Title(LOCTEXT("vector-mass-modifier-window.name", "Mass Modifier"))
        //.ClientSize(FVector2D(800, 400))
        .SizingRule(ESizingRule::Autosized)
        .SupportsMaximize(false)
        .SupportsMinimize(false)
        .IsTopmostWindow(true) // kind-of mimic modal window because we need it to be non-modal for the preview.
        [
            SNew(SVerticalBox)
            +SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            [
                massModifierView
            ]
            +SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            [
                SNew(SButton)
                .Text(LOCTEXT("vector-mass-modifier-window-apply", "Apply"))
                .OnClicked_Raw(this, &SOdysseyAnimationLayerImageTimeline::MassModifierAcceptProperties, massModifierView )
            ]
        ];

        // Ask whether or not to apply modified properties
        ObjectWindow.Get().SetOnWindowClosed( FOnWindowClosed::CreateSP( this
                                                                       , &SOdysseyAnimationLayerImageTimeline::MassModifierWindowClosed
                                                                       , massModifierView ) );

        massModifierView.Get().GetOnPreviewPropertiesDelegate().AddLambda( []()
        {

        } );
/*
        // We don't run a ModalWindow because we need the viewport to redraw for previewing.
        FSlateApplication::Get().AddWindow
        (
            ObjectWindow,
            true
        );
*/
        // We don't run a ModalWindow because we need the viewport to redraw for previewing.
        FSlateApplication::Get().AddModalWindow
        (
            ObjectWindow,
            FGlobalTabmanager::Get()->GetRootWindow(),
            false
        );
/*
        //ObjectWindow.Get().ShowWindow();
*/
        //bMassModifierWindowRunning = true;
    }
}

void
SOdysseyAnimationLayerImageTimeline::MassModifierWindowClosed( const TSharedRef<SWindow>& iWindow
                                                             , TSharedRef<SOdysseyPainterEditorVectorMassModifierView> iMassModifierView )
{
    UOdysseyAnimationLayerImageVector* layerImageVector = Cast<UOdysseyAnimationLayerImageVector>(mLayer->GetLayerStack()->GetCurrentLayer());
    FText dialogText = LOCTEXT( "mass-modifier.apply-properties.title","Apply Properties ?" );

    if( iMassModifierView.Get().HasAnyPropertyBit() )
    {
        iMassModifierView.Get().UndoPreview();

        if( FMessageDialog::Open( EAppMsgType::YesNo, dialogText ) == EAppReturnType::Yes )
        {
            MassModifierAcceptProperties( iMassModifierView );
        }
    }

    //bMassModifierWindowRunning = false;
}

void
SOdysseyAnimationLayerImageTimeline::BuildCellsMarksSubMenu(FMenuBuilder& iMenuBuilder, FFrameNumber iClickedFrame)
{
    iMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.section-clicked.label", "Mark at Frame {0}" ), iClickedFrame.Value + 1 ) );

    iMenuBuilder.AddMenuEntry(
        FUIAction(
            FExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::RemoveCellMarkOnClickedFrame, iClickedFrame ),
            FCanExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::CanRemoveCellMarkOnClickedFrame, iClickedFrame )
        ),
        CreateRemoveCellMarkOnClickedFrameWidget( iClickedFrame ),
        NAME_None,
        TAttribute<FText>::CreateSP( this, &SOdysseyAnimationLayerImageTimeline::GetRemoveCellMarkOnClickedFrameTooltip, iClickedFrame )
    );

    iMenuBuilder.AddSeparator();

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    for( int i = 0; i < settings->AnimationCellsMarks.Num(); i++ )
    {
        FCellMark mark;
        mark.Index = i;
        iMenuBuilder.AddMenuEntry(
            FUIAction(
                FExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::SetCellMarkOnClickedFrame, iClickedFrame, mark ),
                FCanExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::CanSetCellMarkOnClickedFrame, iClickedFrame ),
                FIsActionChecked::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::IsCellMarkCheckedOnClickedFrame, iClickedFrame, mark )
            ),
            CreateCellMarkOnClickedFrameWidget( iClickedFrame, i ),
            NAME_None,
            TAttribute<FText>(),
            EUserInterfaceActionType::RadioButton
        );
    }

    iMenuBuilder.EndSection();

    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "timeline-cells.context-menu.cell-mark.section-selected.label", "Mark in Selected Cells" ) );

    iMenuBuilder.AddMenuEntry(
        LOCTEXT("timeline-cells.context-menu.cell-mark.reset-all.name", "Remove All"),
        LOCTEXT("timeline-cells.context-menu.cell-mark.reset-all.tooltip", "Remove any mark from selected cells"),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::RemoveAllCellMark),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanRemoveAllCellMark)
        )
    );

    //iMenuBuilder.AddSeparator();

    // Keep the functions to manage setting mark on selected cells but only on the first frame

    ////UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    //for (int i = 0; i < settings->AnimationCellsMarks.Num(); i++)
    //{
    //    FCellMark mark;
    //    mark.Index = i;
    //    iMenuBuilder.AddMenuEntry(
    //        FUIAction(
    //            FExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::SetCellMark, mark ),
    //            FCanExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::CanSetCellMark ),
    //            FIsActionChecked::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::IsCellMarkChecked, mark )
    //        ),
    //        CreateCellMarkMenuWidget(i),
    //        NAME_None,
    //        TAttribute<FText>(),
    //        EUserInterfaceActionType::RadioButton
    //    );
    //}

    iMenuBuilder.EndSection();
}

void
SOdysseyAnimationLayerImageTimeline::RemoveAllCellMark()
{
    if (!mLayer->IsEditable())
        return;

    TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.remove-all-marks", "Remove All Marks"));

    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    for (UOdysseyLayerCell* selectedCell : selectedCells)
    {
        selectedCell->SetMarks( {} );
    }
}

bool
SOdysseyAnimationLayerImageTimeline::CanRemoveAllCellMark() const
{
    if (!mLayer->IsEditable())
        return false;

    TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    int mark_count = Algo::TransformAccumulate( selectedCells,
                                                []( const UOdysseyLayerCell* iCell )
                                                {
                                                    return iCell->GetMarks().Num();
                                                },
                                                0 );

    return !!mark_count;
}

//---

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::CreateRemoveCellMarkOnClickedFrameWidget( FFrameNumber iClickedFrame ) const
{
    if( !mLayer->IsEditable() )
        return SNew( STextBlock )
            .Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark" ), iClickedFrame.Value + 1 ) );
            //.Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark at {0}" ), iClickedFrame.Value + 1 ) ); // To display again the frame

    UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( iClickedFrame.Value );
    if( !cell )
        return SNew( STextBlock )
            .Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark" ), iClickedFrame.Value + 1 ) );
            //.Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark at {0}" ), iClickedFrame.Value + 1 ) ); // To display again the frame

    TMap<int, FCellMark> marks = cell->GetMarks();
    int32 index_in_cell = cell->FrameInLayerToIndexInCell( iClickedFrame );

    if( !marks.Contains( index_in_cell ) )
        return SNew( STextBlock )
            .Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark" ), iClickedFrame.Value + 1 ) );
            //.Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark at {0}" ), iClickedFrame.Value + 1 ) ); // To display again the frame

    //---

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[cell->GetMarks()[index_in_cell].Index];
    const FSlateBrush* icon = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
    switch( markSettings.Symbol )
    {
        case EOdysseyAnimationCellMarkSymbol::Triangle: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Triangle" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledTriangle: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Triangle" ); break;
        case EOdysseyAnimationCellMarkSymbol::Circle: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Circle" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledCircle: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Circle" ); break;
        case EOdysseyAnimationCellMarkSymbol::Diamond: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Diamond" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledDiamond: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Diamond" ); break;
        case EOdysseyAnimationCellMarkSymbol::Star: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Star" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledStar: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Star" ); break;
        case EOdysseyAnimationCellMarkSymbol::Cross: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Cross" ); break;
        case EOdysseyAnimationCellMarkSymbol::Checkmark: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Checkmark" ); break;
    }
    FLinearColor iconColor = markSettings.Color;
    FText name = FText::FromName( markSettings.Name );

    return SNew( SHorizontalBox )

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew( STextBlock )
            .Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked.label-remove", "Remove " ), iClickedFrame.Value + 1 ) ) // Give the frame for localisation which maybe display it (with {0}) in this first part
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew( SImage )
            .Image( icon )
            .ColorAndOpacity( iconColor )
        ]

        //+ SHorizontalBox::Slot()
        //.AutoWidth()
        //[
        //    SNew( STextBlock )
        //    .Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked.label-frame", " (at {0})" ), iClickedFrame.Value + 1 ) ) // To display again the frame
        //]
        ;
}

FText
SOdysseyAnimationLayerImageTimeline::GetRemoveCellMarkOnClickedFrameTooltip( FFrameNumber iClickedFrame ) const
{
    UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( iClickedFrame.Value );
    if( !cell )
        return FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.tooltip", "No mark to remove at frame {0}" ), iClickedFrame.Value + 1 );

    TMap<int, FCellMark> marks = cell->GetMarks();
    int32 index_in_cell = cell->FrameInLayerToIndexInCell( iClickedFrame );

    if( !marks.Contains( index_in_cell ) )
        return FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.tooltip", "No mark to remove at frame {0}" ), iClickedFrame.Value + 1 );

    return FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked.tooltip", "Remove mark at frame {0}" ), iClickedFrame.Value + 1 );
}

void
SOdysseyAnimationLayerImageTimeline::RemoveCellMarkOnClickedFrame( FFrameNumber iClickedFrame )
{
    if( !mLayer->IsEditable() )
        return;

    UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( iClickedFrame.Value );
    if( !cell )
        return;

    TMap<int, FCellMark> marks = cell->GetMarks();
    int32 index_in_cell = cell->FrameInLayerToIndexInCell( iClickedFrame );

    if( !marks.Contains( index_in_cell ) )
        return;

    marks.Remove( index_in_cell );
    cell->SetMarks( marks );
}

bool
SOdysseyAnimationLayerImageTimeline::CanRemoveCellMarkOnClickedFrame( FFrameNumber iClickedFrame ) const
{
    if( !mLayer->IsEditable() )
        return false;

    UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( iClickedFrame.Value );
    if( !cell )
        return false;

    TMap<int, FCellMark> marks = cell->GetMarks();
    int32 index_in_cell = cell->FrameInLayerToIndexInCell( iClickedFrame );

    if( !marks.Contains( index_in_cell ) )
        return false;

    return true;
}

//---

//void
//SOdysseyAnimationLayerImageTimeline::SetCellMark( FCellMark iMarkId )
//{
//    if (!mLayer->IsEditable())
//        return;
//
//    TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
//    if (selectedCells.IsEmpty())
//        return;
//
//    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.set-mark", "Set cell mark"));
//
//    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
//    for (UOdysseyLayerCell* selectedCell : selectedCells)
//    {
//        TMap<int, FCellMark> marks = selectedCell->GetMarks();
//        marks.Add( 0, iMarkId );
//        selectedCell->SetMarks( marks );
//    }
//}
//
//bool
//SOdysseyAnimationLayerImageTimeline::CanSetCellMark() const
//{
//    if (!mLayer->IsEditable())
//        return false;
//
//    TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
//    if (selectedCells.IsEmpty())
//        return false;
//
//    return true;
//}
//
//bool
//SOdysseyAnimationLayerImageTimeline::IsCellMarkChecked( FCellMark iMarkId ) const
//{
//    TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
//    if (selectedCells.IsEmpty())
//        return false;
//
//    for (UOdysseyLayerCell* selectedCell : selectedCells)
//    {
//        TMap<int, FCellMark> marks = selectedCell->GetMarks();
//        if( !marks.Contains( 0 ) )
//            return false;
//        if( marks[0].Index != iMarkId.Index )
//            return false;
//    }
//
//    return true;
//}
//
//TSharedRef<SWidget>
//SOdysseyAnimationLayerImageTimeline::CreateCellMarkMenuWidget(int iMarkId)
//{
//    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
//    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[iMarkId];
//    const FSlateBrush* icon = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
//    switch(markSettings.Symbol)
//    {
//        case EOdysseyAnimationCellMarkSymbol::Triangle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Triangle"); break;
//        case EOdysseyAnimationCellMarkSymbol::FilledTriangle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Triangle"); break;
//        case EOdysseyAnimationCellMarkSymbol::Circle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Circle"); break;
//        case EOdysseyAnimationCellMarkSymbol::FilledCircle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Circle"); break;
//        case EOdysseyAnimationCellMarkSymbol::Diamond: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Diamond"); break;
//        case EOdysseyAnimationCellMarkSymbol::FilledDiamond: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Diamond"); break;
//        case EOdysseyAnimationCellMarkSymbol::Star: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Star"); break;
//        case EOdysseyAnimationCellMarkSymbol::FilledStar: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Star"); break;
//        case EOdysseyAnimationCellMarkSymbol::Cross: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Cross"); break;
//        case EOdysseyAnimationCellMarkSymbol::Checkmark: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Checkmark"); break;
//    }
//    FLinearColor iconColor = markSettings.Color;
//    FText name = FText::FromName(markSettings.Name);
//
//    return SNew(SHorizontalBox)
//    + SHorizontalBox::Slot()
//    .Padding(FMargin(0, 0, 4, 0))
//    .AutoWidth()
//    [
//        SNew(SImage)
//        .Image(icon)
//        .ColorAndOpacity(iconColor)
//    ]
//    + SHorizontalBox::Slot()
//    .AutoWidth()
//    [
//        SNew(STextBlock)
//        .Text(name)
//    ];
//}

//---

void
SOdysseyAnimationLayerImageTimeline::SetCellMarkOnClickedFrame( FFrameNumber iClickedFrame, FCellMark iMarkId )
{
    if( !mLayer->IsEditable() )
        return;

    UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( iClickedFrame.Value );
    if( !cell )
        return;

    TMap<int, FCellMark> marks = cell->GetMarks();
    int32 index_in_cell = cell->FrameInLayerToIndexInCell( iClickedFrame );

    FScopedTransaction ScopedTransaction( LOCTEXT( "timeline-cells.transaction.set-mark-clicked", "Set cell mark on clicked frame" ) );

    marks.Add( index_in_cell, iMarkId );
    cell->SetMarks( marks );
}

bool
SOdysseyAnimationLayerImageTimeline::CanSetCellMarkOnClickedFrame( FFrameNumber iClickedFrame ) const
{
    if( !mLayer->IsEditable() )
        return false;

    UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( iClickedFrame.Value );
    if( !cell )
        return false;

    //TMap<int, FCellMark> marks = cell->GetMarks();
    //int32 index_in_cell = cell->FrameInLayerToIndexInCell( iClickedFrame );

    //if( !marks.Contains( index_in_cell ) )
    //    return false;

    return true;
}

bool
SOdysseyAnimationLayerImageTimeline::IsCellMarkCheckedOnClickedFrame( FFrameNumber iClickedFrame, FCellMark iMarkId ) const
{
    if( !mLayer->IsEditable() )
        return false;

    UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( iClickedFrame.Value );
    if( !cell )
        return false;

    TMap<int, FCellMark> marks = cell->GetMarks();
    int32 index_in_cell = cell->FrameInLayerToIndexInCell( iClickedFrame );

    if( !marks.Contains( index_in_cell ) )
        return false;

    if( marks[index_in_cell].Index != iMarkId.Index )
        return false;

    return true;
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::CreateCellMarkOnClickedFrameWidget( FFrameNumber iClickedFrame, int iMarkId )
{
    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[iMarkId];
    const FSlateBrush* icon = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
    switch( markSettings.Symbol )
    {
        case EOdysseyAnimationCellMarkSymbol::Triangle: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Triangle" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledTriangle: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Triangle" ); break;
        case EOdysseyAnimationCellMarkSymbol::Circle: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Circle" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledCircle: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Circle" ); break;
        case EOdysseyAnimationCellMarkSymbol::Diamond: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Diamond" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledDiamond: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Diamond" ); break;
        case EOdysseyAnimationCellMarkSymbol::Star: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Star" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledStar: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Star" ); break;
        case EOdysseyAnimationCellMarkSymbol::Cross: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Cross" ); break;
        case EOdysseyAnimationCellMarkSymbol::Checkmark: icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Checkmark" ); break;
    }
    FLinearColor iconColor = markSettings.Color;
    FText name = FText::FromName( markSettings.Name );

    return SNew( SHorizontalBox )
        + SHorizontalBox::Slot()
        .Padding( FMargin( 0, 0, 4, 0 ) )
        .AutoWidth()
        [
            SNew( SImage )
            .Image( icon )
            .ColorAndOpacity( iconColor )
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew( STextBlock )
            .Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.set-clicked.label-frame", "{0}" ), name, iClickedFrame.Value + 1 ) )
            //.Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.set-clicked.label-frame", "{0} (at {1})" ), name, iClickedFrame.Value + 1 ) ) // To display again the frame
        ];
}

#undef LOCTEXT_NAMESPACE
