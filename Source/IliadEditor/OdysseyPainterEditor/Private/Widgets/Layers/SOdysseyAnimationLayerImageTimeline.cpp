// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationLayerImageTimeline.h"

#include "Algo/Accumulate.h"
#include "Editor.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/GenericCommands.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "ISinglePropertyView.h"
#include "Layout/WidgetPath.h"
#include "Modules/ModuleManager.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "ScopedTransaction.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SEnableBox.h"

#include "OdysseyAnimationCellsDragDropOperation.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyLayerCellImageStagger.h"
#include "OdysseyLayerStackSelection.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyPainterEditorAnimationProjectSettings.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyPainterEditorAnimationUserSettings.h"
#include "OdysseyStyle.h"
#include "OdysseyVectorGroupPaint.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageStaggerShortcuts.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellsShortcuts.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalCellsShortcuts.h"
#include "SOdysseyAnimationTimelineCellNames.h"
#include "SOdysseyAnimationTimelineLighttable.h"
#include "SOdysseyAnimationTimelineOutOfPegs.h"
#include "SOdysseyAnimationTimelineScrollBox.h"
#include "SOdysseyAnimationCells.h"
#include "TimelineTools/OdysseyAnimationTimelineTool.h"
#include "TimelineTools/OdysseyAnimationTimelineTools.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/SOdysseyEvents.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorMassModifierView.h"

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
    return SNew(SOdysseyAnimationTimelineCellNames)
        .Cells_Lambda(
            [this]() -> TArray<UOdysseyLayerCell*>
            {
                return mLayer->GetCells();
            }
        )
        .TimelinePosition(mTimelinePosition);
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

    mTool = FOdysseyAnimationTimelineTools::Get().CreateTool(mTimelinePosition.ToSharedRef());
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

        const TArray<UOdysseyLayerCell*> selectedCells = OdysseyLayerStackSelection::GetSelectedCells(mLayer);
        if (selectedCells.IsEmpty() || !selectedCells.Contains(cell))
        {
            OdysseyLayerStackSelection::Get()->DeselectAll();
            OdysseyLayerStackSelection::Get()->Select(cell);
        }

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

        mAnimationTimelineCellsShortcuts = MakeShared<FOdysseyAnimationTimelineCellsShortcuts>( mLayer->GetAnimation(), mCurrentFrame );
        mAnimationTimelineCellsShortcuts->MapActionsToCommandList( commandList );

        mAnimationGlobalCellsShortcuts = MakeShared<FOdysseyAnimationGlobalCellsShortcuts>( mLayer->GetAnimation(), mCurrentFrame );
        mAnimationGlobalCellsShortcuts->MapActionsToCommandList( commandList );

        mAnimationTimelineCellImageStaggerShortcuts = MakeShared<FOdysseyAnimationTimelineCellImageStaggerShortcuts>( mLayer->GetAnimation() );
        mAnimationTimelineCellImageStaggerShortcuts->MapActionsToCommandList( commandList );

        TSharedRef<FExtender> MenuExtender = MakeShared<FExtender>();
        FMenuBuilder menuBuilder(true, commandList, MenuExtender);

        BuildContextMenu(commandList, menuBuilder, frame, MenuExtender);

        TSharedRef<SWidget> menuContents = menuBuilder.MakeWidget();
        FWidgetPath widgetPath = iEvent.GetEventPath() != nullptr ? *iEvent.GetEventPath() : FWidgetPath();
        FSlateApplication::Get().PushMenu(mEventWidgets["Main"].ToSharedRef(), widgetPath, menuContents, iEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
        return FReply::Handled();
    }

    return FReply::Unhandled().ReleaseMouseCapture();
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

        TSharedPtr<FOdysseyAnimationTimelineTool> tool = FOdysseyAnimationTimelineTools::Get().CreateTool( mTimelinePosition.ToSharedRef() );
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
    if (cell)
    {
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
    }
    else
    {
        //We are outside the cells limits
        mDragPosition = FMath::Max(0, int(frame + 0.5f));

        /*
        if (frame < mLayer->GetFrameRange().GetLowerBoundValue())
        {
            //We are before the first cell
            mDragPosition = mLayer->GetFrameRange().GetLowerBoundValue();
        }
        else if (frame > mLayer->GetFrameRange().GetUpperBoundValue())
        {
            //We are after the last cell
            mDragPosition = mLayer->GetFrameRange().GetUpperBoundValue() + 1;
        }
        */
    }

    if (FSlateApplication::Get().GetModifierKeys().IsShiftDown())
        mDragState = kDrag_Copy;
    else
        mDragState = kDrag_Move;

    //Check if the copy or move is actually allowed
    UOdysseyAnimationLayer* layer = operation->GetLayer();
    if (layer == mLayer)
    {
        const TArray<UOdysseyLayerCell*> selectedCells = OdysseyLayerStackSelection::GetSelectedCells(layer);
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
    else
    {
        //We force a copy if source layer and destination layer are different
        //Because Moving Cells from one layer to another is weird, so we don't allow it
        mDragState = kDrag_Copy;
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
    else if (mDragState == kDrag_Move)
    {
        FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.dnd-move", "Move Cells"));
        operation->GetData().Move(mLayer, mDragPosition);
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
SOdysseyAnimationLayerImageTimeline::BuildContextMenu(TSharedRef<FUICommandList> CommandList, FMenuBuilder& MenuBuilder, FFrameNumber iClickedFrame, TSharedRef<FExtender> MenuExtender)
{
    MenuBuilder.BeginSection("Selection", LOCTEXT("timeline-cells.context-menu.selection-section.name", "Selection"));
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().SelectAll);
    MenuBuilder.EndSection();

    //---

    MenuBuilder.BeginSection("Common", LOCTEXT("timeline-cells.context-menu.common-section.name", "Common"));
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Cut);
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Paste);
        MenuBuilder.AddSeparator();
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
    MenuBuilder.EndSection();

    //---

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

            const TArray<UOdysseyLayerCell*> selectedCells = OdysseyLayerStackSelection::GetSelectedCells(layer);
            if( selectedCells.IsEmpty() )
            {
                UOdysseyLayerCell* cell = layer->GetCellAtFrame( mCurrentFrame.Get() );
                if( !cell )
                    return true;
            }

            return false;
        };

    MenuBuilder.BeginSection("Cells", LOCTEXT("timeline-cells.context-menu.cells-section.name", "Cells"));
        //PATCH: Without this separator, the following AddEditableText() will be added at the end of the previous section oO
        // maybe try to investigate ... one day ... -_-
        MenuBuilder.AddSeparator( NAME_None, EVisibility::Collapsed );
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
                                                 const TArray<UOdysseyLayerCell*> selectedCells = OdysseyLayerStackSelection::GetSelectedCells(mLayer);
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

                                                const TArray<UOdysseyLayerCell*> selectedCells = OdysseyLayerStackSelection::GetSelectedCells(mLayer);
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
        MenuBuilder.AddSubMenu(
            LOCTEXT( "timeline-cells.context-menu.create-stagger-cell.name", "Stagger" ),
            LOCTEXT( "timeline-cells.context-menu.create-stagger-cell.tooltip", "Create stagger cell from selected cells" ),
            FNewMenuDelegate::CreateLambda( [this]( FMenuBuilder& ioMenuBuilder )
                                            {
                                                ioMenuBuilder.AddMenuEntry(FOdysseyPainterEditorAnimationCommands::Get().CreateStaggerCellLoop, NAME_None, LOCTEXT("timeline-cells.context-menu.stagger-cell-loop.name", "Loop"));
                                                ioMenuBuilder.AddMenuEntry(FOdysseyPainterEditorAnimationCommands::Get().CreateStaggerCellPingPong, NAME_None, LOCTEXT("timeline-cells.context-menu.stagger-cell-pingpong.name", "Ping-Pong"));
                                                ioMenuBuilder.AddMenuEntry(FOdysseyPainterEditorAnimationCommands::Get().CreateStaggerCellRandom, NAME_None, LOCTEXT("timeline-cells.context-menu.stagger-cell-random.name", "Random"));
                                                ioMenuBuilder.AddSeparator();
                                                ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorAnimationCommands::Get().ConvertToReferenceCells );
                                            } ),
            false, // bInOpenSubMenuOnClick
            FSlateIcon(),
            true, // bInShouldCloseWindowAfterMenuSelection
            "CreateStagger"
        );
        MenuBuilder.AddMenuEntry(
              FOdysseyPainterEditorAnimationCommands::Get().ReverseSelectedCells
            , NAME_None
            , LOCTEXT("timeline-cells.context-menu.reverse-selected-cells.name", "Reverse Selected Cells")
            , LOCTEXT("timeline-cells.context-menu.reverse-selected-cells.tooltip", "Reverse the order of the selected cells")
            , FSlateIcon()
        );
    MenuBuilder.EndSection();

    //---

    UOdysseyPainterEditorAnimationUserSettings* Settings = GetMutableDefault<UOdysseyPainterEditorAnimationUserSettings>();

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );

    FSinglePropertyParams CellsParams;
    CellsParams.NamePlacement = EPropertyNamePlacement::Hidden;
    CellsParams.bHideResetToDefault = true;

    TSharedPtr<ISinglePropertyView> CellsPropertyView = PropertyEditorModule.CreateSingleProperty( Settings, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorAnimationUserSettings, NumberOfCellsToAdd ), CellsParams );

    FSlimHorizontalToolBarBuilder AddCellsBeforeBuilder( CommandList, FMultiBoxCustomization::None, nullptr, true );
    AddCellsBeforeBuilder.SetStyle( &FOdysseyStyle::Get(), "InnerMenuToolBar.LargeIcon" );
    AddCellsBeforeBuilder.AddToolBarButton( FOdysseyPainterEditorAnimationCommands::Get().AddCellsBefore, NAME_None );

    FSlimHorizontalToolBarBuilder AddCellsAfter( CommandList, FMultiBoxCustomization::None, nullptr, true );
    AddCellsAfter.SetStyle( &FOdysseyStyle::Get(), "InnerMenuToolBar.LargeIcon" );
    AddCellsAfter.AddToolBarButton( FOdysseyPainterEditorAnimationCommands::Get().AddCellsAfter, NAME_None );

    MenuBuilder.BeginSection( "AddCells", LOCTEXT( "timeline-cells.context-menu.add-cells-section.name", "Add Cells" ) );

        MenuBuilder.AddWidget(
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding( FMargin( 4, 0, 2, 0 ) )
            [
                AddCellsBeforeBuilder.MakeWidget()
            ]
            + SHorizontalBox::Slot()
            .FillWidth( 1 )
            .HAlign( HAlign_Fill )
            [
                CellsPropertyView.ToSharedRef()
            ]
            //+ SHorizontalBox::Slot()
            //.FillWidth( .7 )
            //.HAlign( HAlign_Fill )
            //.VAlign( VAlign_Center )
            //[
            //    SNew( STextBlock )
            //    //TODO: if needed, make it as lambda with static format
            //    .Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cells-unit", "{0}|plural(one=cell,other=cells)" ), GetDefault<UOdysseyPainterEditorAnimationUserSettings>()->NumberOfCellsToAdd ) )
            //    //.Text( LOCTEXT( "timeline-cells.context-menu.cells-unit", "cell" ) )
            //]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding( FMargin( 2, 0, 4, 0 ) )
            [
                AddCellsAfter.MakeWidget()
            ],
            FText(),
            true,
            false
        );

    MenuBuilder.EndSection();

    //---

    FSinglePropertyParams ExposuresParams;
    ExposuresParams.NamePlacement = EPropertyNamePlacement::Hidden;
    ExposuresParams.bHideResetToDefault = true;

    TSharedPtr<ISinglePropertyView> ExposuresPropertyView = PropertyEditorModule.CreateSingleProperty( Settings, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorAnimationUserSettings, NumberOfExposuresToIncreaseOrDecrease ), ExposuresParams );

    FSlimHorizontalToolBarBuilder IncreaseButtonBuilder( CommandList, FMultiBoxCustomization::None, nullptr, true );
    IncreaseButtonBuilder.SetStyle( &FOdysseyStyle::Get(), "InnerMenuToolBar.LargeIcon" );
    IncreaseButtonBuilder.AddToolBarButton( FOdysseyPainterEditorAnimationCommands::Get().IncreaseNCellExposure, NAME_None );

    FSlimHorizontalToolBarBuilder DecreaseButtonBuilder( CommandList, FMultiBoxCustomization::None, nullptr, true );
    DecreaseButtonBuilder.SetStyle( &FOdysseyStyle::Get(), "InnerMenuToolBar.LargeIcon" );
    DecreaseButtonBuilder.AddToolBarButton( FOdysseyPainterEditorAnimationCommands::Get().DecreaseNCellExposure, NAME_None );

    MenuBuilder.BeginSection( "AddExposure", LOCTEXT( "timeline-cells.context-menu.add-exposure-section.name", "Add/Remove Exposure" ) );

        MenuBuilder.AddWidget(
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding( FMargin( 4, 0, 2, 0 ) )
            [
                DecreaseButtonBuilder.MakeWidget()
            ]
            + SHorizontalBox::Slot()
            .FillWidth( 1 )
            .HAlign( HAlign_Fill )
            [
                ExposuresPropertyView.ToSharedRef()
            ]
            //+ SHorizontalBox::Slot()
            //.FillWidth( .7 )
            //.HAlign( HAlign_Fill )
            //.VAlign( VAlign_Center )
            //[
            //    SNew( STextBlock )
            //    .Text( LOCTEXT( "timeline-cells.context-menu.exposures-unit", "exp." ) )
            //]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding( FMargin( 2, 0, 4, 0 ) )
            [
                IncreaseButtonBuilder.MakeWidget()
            ],
            FText(),
            true,
            false
        );

    MenuBuilder.EndSection();

    //---

    MenuBuilder.BeginSection( "SetExposure", LOCTEXT( "timeline-cells.context-menu.set-exposure-section.name", "Set Exposure" ) );

        MenuBuilder.AddWidget(
            SNew( SSpinBox<uint32> )
            .ToolTipText( LOCTEXT( "timeline-cells.context-menu.set-exposure.tooltip", "Set the exposure of the current or selected cells." ) )
            .Justification( ETextJustify::Center )
            .MinDesiredWidth( 50.f )
            //.PreventThrottling( true ) // To refresh the viewport during value change
            .Delta( 1.f )
            //.SliderExponent( 0.8f )
            .IsEnabled( this, &SOdysseyAnimationLayerImageTimeline::IsCurrentExposureEnabled )
            .Value( this, &SOdysseyAnimationLayerImageTimeline::GetCurrentExposure )
            .OnBeginSliderMovement( this, &SOdysseyAnimationLayerImageTimeline::BeginSetCurrentExposureTransaction )
            .OnValueChanged( this, &SOdysseyAnimationLayerImageTimeline::SetCurrentExposureInteractive )
            .OnEndSliderMovement( this, &SOdysseyAnimationLayerImageTimeline::EndSetCurrentExposureTransaction )
            .OnValueCommitted( this, &SOdysseyAnimationLayerImageTimeline::SetCurrentExposureCommitted ),
            LOCTEXT( "timeline-cells.context-menu.set-exposure.label", "Set Exposure" ),
            //FText::GetEmpty(),
            true /* NoIndent */
        );

    MenuBuilder.EndSection();
}

bool
SOdysseyAnimationLayerImageTimeline::IsCurrentExposureEnabled() const
{
    TArray<UOdysseyLayerCell*> selectedCells = OdysseyLayerStackSelection::GetSelectedCells(mLayer);
    if( selectedCells.IsEmpty() )
    {
        UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( mCurrentFrame.Get() );
        if( !cell )
            return false;

        selectedCells.Add( cell );
    }

    return !!selectedCells.Num();
}
uint32
SOdysseyAnimationLayerImageTimeline::GetCurrentExposure() const
{
    UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( mCurrentFrame.Get() );

    TArray<UOdysseyLayerCell*> selectedCells = OdysseyLayerStackSelection::GetSelectedCells(mLayer);
    if( selectedCells.IsEmpty() )
    {
        if( !cell )
            return 0u;

        selectedCells.Add( cell );
    }
    check( selectedCells.Num() );

    return selectedCells.Contains( cell ) ? cell->GetExposure() : selectedCells[0]->GetExposure();
}
void
SOdysseyAnimationLayerImageTimeline::BeginSetCurrentExposureTransaction()
{
    if( !mLayer->IsEditable() )
        return;

    GEditor->BeginTransaction( LOCTEXT( "timeline-cells.transaction.set-selected-cells-exposure", "Set Selected Cells Exposure" ) );
}
void
SOdysseyAnimationLayerImageTimeline::SetCurrentExposureInteractive( uint32 iNewExposure )
{
    if( !mLayer->IsEditable() )
        return;

    SetCurrentExposure( iNewExposure );
}
void
SOdysseyAnimationLayerImageTimeline::EndSetCurrentExposureTransaction( uint32 iNewExposure )
{
    if( !mLayer->IsEditable() )
        return;

    SetCurrentExposure( iNewExposure );
    TArray<UOdysseyLayerCell*> selectedCells = OdysseyLayerStackSelection::GetSelectedCells(mLayer);
    OdysseyLayerStackSelection::RegisterUndo(selectedCells, selectedCells);
    GEditor->EndTransaction();
}
void
SOdysseyAnimationLayerImageTimeline::SetCurrentExposureCommitted( uint32 iNewExposure, ETextCommit::Type iCommitType )
{
    if( iCommitType == ETextCommit::OnCleared )
        return;

    if( !mLayer->IsEditable() )
        return;

    FScopedTransaction ScopedTransaction( LOCTEXT( "timeline-cells.transaction.set-selected-cells-exposure", "Set Selected Cells Exposure" ) );

    SetCurrentExposure( iNewExposure );
    TArray<UOdysseyLayerCell*> selectedCells = OdysseyLayerStackSelection::GetSelectedCells(mLayer);
    OdysseyLayerStackSelection::RegisterUndo(selectedCells, selectedCells);
}
void
SOdysseyAnimationLayerImageTimeline::SetCurrentExposure( uint32 iNewExposure )
{
    check( mLayer->IsEditable() );

    TArray<UOdysseyLayerCell*> selectedCells = OdysseyLayerStackSelection::GetSelectedCells(mLayer);
    if( selectedCells.IsEmpty() )
    {
        UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( mCurrentFrame.Get() );
        if( !cell )
            return;

        selectedCells.Add( cell );
    }

    for( UOdysseyLayerCell* selectedCell : selectedCells )
    {
        selectedCell->SetExposure( FMath::Max( 1u, iNewExposure ) );
    }
}

EVisibility
SOdysseyAnimationLayerImageTimeline::GetSingleCellMarkVisibility( ECellMarkApplyType iCellMarkApplyType ) const
{
    const UOdysseyPainterEditorSettings* settings = GetDefault<UOdysseyPainterEditorSettings>();

    return iCellMarkApplyType == ECellMarkApplyType::MarkCellOnSingleFrame && settings->MarkSingleFrame
        || iCellMarkApplyType == ECellMarkApplyType::MarkCellOnCellFirstFrame && !settings->MarkSingleFrame
        ? EVisibility::Visible
        : EVisibility::Collapsed;
}

void
SOdysseyAnimationLayerImageTimeline::BuildCellsMarksSubMenu(FMenuBuilder& iMenuBuilder, FFrameNumber iClickedFrame)
{
    FMenuEntryParams settingsSingleCellMarkParams;
    settingsSingleCellMarkParams.DirectActions = FUIAction(
        FExecuteAction::CreateLambda( [this]() -> void
                                      {
                                          UOdysseyPainterEditorSettings* settings = GetMutableDefault<UOdysseyPainterEditorSettings>();

                                          settings->MarkSingleFrame = true;
                                          settings->SaveConfig();
                                      } ),
        FCanExecuteAction(),
        FIsActionChecked::CreateLambda( [this]() -> bool
                                        {
                                            const UOdysseyPainterEditorSettings* settings = GetDefault<UOdysseyPainterEditorSettings>();

                                            return settings->MarkSingleFrame;
                                        } )
    );
    settingsSingleCellMarkParams.LabelOverride = LOCTEXT( "timeline-cells.context-menu.cell-mark.mark-single-frame.label", "Mark Single Frame" );
    settingsSingleCellMarkParams.ToolTipOverride = LOCTEXT( "timeline-cells.context-menu.cell-mark.mark-single-frame.tooltip", "Each frame in a cell can be marked but only one frame can be marked at a time" );
    settingsSingleCellMarkParams.UserInterfaceActionType = EUserInterfaceActionType::RadioButton;
    iMenuBuilder.AddMenuEntry( settingsSingleCellMarkParams );

    FMenuEntryParams settingsCellMarkParams;
    settingsCellMarkParams.DirectActions = FUIAction(
        FExecuteAction::CreateLambda( [this]() -> void
                                      {
                                          UOdysseyPainterEditorSettings* settings = GetMutableDefault<UOdysseyPainterEditorSettings>();

                                          settings->MarkSingleFrame = false;
                                          settings->SaveConfig();
                                      } ),
        FCanExecuteAction(),
        FIsActionChecked::CreateLambda( [this]() -> bool
                                        {
                                            const UOdysseyPainterEditorSettings* settings = GetDefault<UOdysseyPainterEditorSettings>();

                                            return !settings->MarkSingleFrame;
                                        } )
    );
    settingsCellMarkParams.LabelOverride = LOCTEXT( "timeline-cells.context-menu.cell-mark.mark-cells.label", "Mark Cells" );
    settingsCellMarkParams.ToolTipOverride = LOCTEXT( "timeline-cells.context-menu.cell-mark.mark-cells.tooltip", "Mark only the first frame of cells, then multiple cells can be marked at a time" );
    settingsCellMarkParams.UserInterfaceActionType = EUserInterfaceActionType::RadioButton;
    iMenuBuilder.AddMenuEntry( settingsCellMarkParams );

    //---

    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "timeline-cells.context-menu.cell-mark.section-clicked.label", "Current Marks" ) );
    {
        FMenuEntryParams removeCellMarkParams;
        removeCellMarkParams.DirectActions = FUIAction(
            FExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::RemoveCellMarkOnClickedFrame, iClickedFrame ),
            FCanExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::CanRemoveCellMarkOnClickedFrame, iClickedFrame )
        );
        removeCellMarkParams.ToolTipOverride = MakeAttributeSP( this, &SOdysseyAnimationLayerImageTimeline::GetRemoveCellMarkOnClickedFrameTooltip, iClickedFrame );
        removeCellMarkParams.EntryWidget = CreateRemoveCellMarkOnClickedFrameWidget( iClickedFrame );
        removeCellMarkParams.Visibility = MakeAttributeSP( this, &SOdysseyAnimationLayerImageTimeline::GetSingleCellMarkVisibility, ECellMarkApplyType::MarkCellOnSingleFrame );
        iMenuBuilder.AddMenuEntry( removeCellMarkParams );

        iMenuBuilder.AddSeparator( NAME_None, MakeAttributeSP( this, &SOdysseyAnimationLayerImageTimeline::GetSingleCellMarkVisibility, ECellMarkApplyType::MarkCellOnSingleFrame ) );

        // Can't use FOdysseyAnimationGlobalCellsShortcuts commands to auto-create menu entries
        // as they rely on current frame and not on clicked frame
        UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
        for( int i = 0; i < settings->AnimationCellsMarks.Num(); i++ )
        {
            FCellMark mark;
            mark.Index = i;

            FMenuEntryParams addCellMarkParams;
            addCellMarkParams.DirectActions = FUIAction(
                FExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::SetCellMarkOnClickedFrame, iClickedFrame, mark ),
                FCanExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::CanSetCellMarkOnClickedFrame, iClickedFrame ),
                FIsActionChecked::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::IsCellMarkCheckedOnClickedFrame, iClickedFrame, mark )
            );
            addCellMarkParams.EntryWidget = CreateCellMarkOnClickedFrameWidget( iClickedFrame, i );
            addCellMarkParams.UserInterfaceActionType = EUserInterfaceActionType::RadioButton;
            addCellMarkParams.Visibility = MakeAttributeSP( this, &SOdysseyAnimationLayerImageTimeline::GetSingleCellMarkVisibility, ECellMarkApplyType::MarkCellOnSingleFrame );

            iMenuBuilder.AddMenuEntry( addCellMarkParams );
        }

        for( int markIndex = 0; markIndex < settings->AnimationCellsMarks.Num(); markIndex++ )
        {
            // Can't use iMenuBuilder.AddMenuEntry with a FUICommandInfo parameter as it only take a FSlateIcon
            // and in our case, the icon must be tint with a specific color
            // but then, shortcut keys are not displayed :[

            //FMenuEntryParams addCellMarkParams;
            //addCellMarkParams.DirectActions = FUIAction(
            //    FExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::SetCellMarkOnCellFirstFrame, markIndex ),
            //    FCanExecuteAction::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::CanSetCellMarkOnCellFirstFrame, markIndex ),
            //    FIsActionChecked::CreateRaw( this, &SOdysseyAnimationLayerImageTimeline::IsCellMarkCheckedOnCellFirstFrame, markIndex )
            //);
            //addCellMarkParams.EntryWidget = CreateCellMarkOnCellFirstFrameWidget( markIndex );
            //addCellMarkParams.UserInterfaceActionType = EUserInterfaceActionType::Check;
            //addCellMarkParams.Visibility = MakeAttributeSP( this, &SOdysseyAnimationLayerImageTimeline::GetSingleCellMarkVisibility, ECellMarkApplyType::MarkCellOnCellFirstFrame );

            //iMenuBuilder.AddMenuEntry( addCellMarkParams );

            //--

            FMenuEntryParams addCellMarkParams;
            *((TSharedPtr< const FUICommandInfo >*)&addCellMarkParams.Action) = FOdysseyPainterEditorAnimationCommands::Get().SetCellMark[markIndex];
            // With Action, ActionList must be provided
            // I don't know why it doesn't use the one in FMenuBuilder (?)
            *((TSharedPtr< const FUICommandList >*)&addCellMarkParams.ActionList) = iMenuBuilder.GetTopCommandList();
            // By using a custom widget, the shortcut won't be displayed -_-
            addCellMarkParams.EntryWidget = CreateCellMarkOnCellFirstFrameWidget( markIndex );
            //addCellMarkParams.LabelOverride = ;
            //addCellMarkParams.IconOverride = ; // Can't create a dynamic icon with custom color -_- to avoid using a custom widget
            addCellMarkParams.Visibility = MakeAttributeSP( this, &SOdysseyAnimationLayerImageTimeline::GetSingleCellMarkVisibility, ECellMarkApplyType::MarkCellOnCellFirstFrame );

            iMenuBuilder.AddMenuEntry( addCellMarkParams );
        }
    }
    iMenuBuilder.EndSection();

    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "timeline-cells.context-menu.cell-mark.section-selected.label", "Mark in Selected Cells" ) );
    {
        iMenuBuilder.AddMenuEntry(
            FOdysseyPainterEditorAnimationCommands::Get().RemoveAllCellMarks,
            NAME_None
        );
    }
    iMenuBuilder.EndSection();
}

//---

static
const FSlateBrush*
GetBrushFromMarkSymbol( EOdysseyAnimationCellMarkSymbol iSymbol )
{
    const FSlateBrush* icon = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
    switch( iSymbol )
    {
        case EOdysseyAnimationCellMarkSymbol::Triangle:         icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Triangle" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledTriangle:   icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Triangle" ); break;
        case EOdysseyAnimationCellMarkSymbol::Circle:           icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Circle" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledCircle:     icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Circle" ); break;
        case EOdysseyAnimationCellMarkSymbol::Diamond:          icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Diamond" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledDiamond:    icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Diamond" ); break;
        case EOdysseyAnimationCellMarkSymbol::Star:             icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Star" ); break;
        case EOdysseyAnimationCellMarkSymbol::FilledStar:       icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Filled.Star" ); break;
        case EOdysseyAnimationCellMarkSymbol::Cross:            icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Cross" ); break;
        case EOdysseyAnimationCellMarkSymbol::Checkmark:        icon = FOdysseyStyle::GetBrush( "Animation.CellMark.Symbol.Checkmark" ); break;
    }

    return icon;
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::CreateRemoveCellMarkOnClickedFrameWidget( FFrameNumber iClickedFrame ) const
{
    if( !mLayer->IsEditable() )
        return SNew( STextBlock )
            .Text( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark" ) );
            //.Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark at {0}" ), iClickedFrame.Value + 1 ) ); // To display again the frame

    UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( iClickedFrame.Value );
    if( !cell )
        return SNew( STextBlock )
            .Text( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark" ) );
            //.Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark at {0}" ), iClickedFrame.Value + 1 ) ); // To display again the frame

    TMap<int, FCellMark> marks = cell->GetMarks();
    int32 index_in_cell = cell->FrameInLayerToIndexInCell( iClickedFrame );

    if( !marks.Contains( index_in_cell ) )
        return SNew( STextBlock )
            .Text( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark" ) );
            //.Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.reset-clicked-nothing.label", "No Mark at {0}" ), iClickedFrame.Value + 1 ) ); // To display again the frame

    //---

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[cell->GetMarks()[index_in_cell].Index];

    const FSlateBrush* icon = GetBrushFromMarkSymbol( markSettings.Symbol );
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

    const FSlateBrush* icon = GetBrushFromMarkSymbol( markSettings.Symbol );
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

//---

void
SOdysseyAnimationLayerImageTimeline::SetCellMarkOnCellFirstFrame( int iMarkId )
{
    mAnimationGlobalCellsShortcuts->Action_SetCellMark( iMarkId );
}
bool
SOdysseyAnimationLayerImageTimeline::CanSetCellMarkOnCellFirstFrame( int iMarkId ) const
{
    return mAnimationGlobalCellsShortcuts->CanAction_SetCellMark( iMarkId );
}
bool
SOdysseyAnimationLayerImageTimeline::IsCellMarkCheckedOnCellFirstFrame( int iMarkId ) const
{
    return mAnimationGlobalCellsShortcuts->IsActionChecked_SetCellMark( iMarkId );
}
TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::CreateCellMarkOnCellFirstFrameWidget( int iMarkId )
{
    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[iMarkId];

    const FSlateBrush* icon = GetBrushFromMarkSymbol( markSettings.Symbol );
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
            .Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.set-cell-first-frame.label-frame", "{0}" ), name ) )
            //.Text( FText::Format( LOCTEXT( "timeline-cells.context-menu.cell-mark.set-clicked.label-frame", "{0} (at {1})" ), name, iClickedFrame.Value + 1 ) ) // To display again the frame
        ];
}


#undef LOCTEXT_NAMESPACE
