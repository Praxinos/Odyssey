// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerImageTimeline.h"
#include "OdysseyLayerCellImageStagger.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLighttable.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineOutOfPegs.h"
#include "Widgets/Animation/Timeline/Cells/SOdysseyAnimationCells.h"
#include "TimelineTools/OdysseyAnimationTimelineTool.h"
#include "OdysseyAnimationCellsDragDropOperation.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationCellImageVector.h"
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

        //Open the context menu
        TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
        FMenuBuilder menuBuilder(true, commandList);

        BuildContextMenu(commandList, menuBuilder);

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
    #if WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.dnd-copy", "Copy Cells"));
    #endif
        operation->GetData().Paste(mLayer, mDragPosition);
    }
    mIsDraggingOver = false;
    return FReply::Handled();
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

void
SOdysseyAnimationLayerImageTimeline::BuildContextMenu(TSharedRef<FUICommandList> CommandList, FMenuBuilder& MenuBuilder)
{
    mAnimationTimelineCellsShortcuts = MakeShared<FOdysseyAnimationTimelineCellsShortcuts>(mLayer->GetAnimation(), mCurrentFrame, mOnTransactCurrentFrame);
    mAnimationTimelineCellsShortcuts->MapActionsToCommandList(CommandList);

    mAnimationTimelineCellImageStaggerShortcuts = MakeShared<FOdysseyAnimationTimelineCellImageStaggerShortcuts>(mLayer->GetAnimation());
    mAnimationTimelineCellImageStaggerShortcuts->MapActionsToCommandList(CommandList);

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
        MenuBuilder.AddMenuEntry(FOdysseyPainterEditorAnimationCommands::Get().ConvertToStaggerCell, TEXT("ConvertToStagger"));
        MenuBuilder.AddMenuEntry(FOdysseyPainterEditorAnimationCommands::Get().ConvertToReferenceCells);
        MenuBuilder.AddMenuEntry(
            FOdysseyPainterEditorAnimationCommands::Get().SetCellExposure,
            NAME_None,
            LOCTEXT("timeline-cells.context-menu.set-selected-cells-exposure.name", "Set Exposure")
        );
        MenuBuilder.AddSubMenu(
            LOCTEXT("timeline-cells.context-menu.cell-mark.name", "Mark"),
            LOCTEXT("timeline-cells.context-menu.cell-mark.tooltip", "Set a mark on the selected cells"),
            FNewMenuDelegate::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::BuildCellsMarksSubMenu)
        );
        MenuBuilder.AddMenuEntry(
              LOCTEXT("timeline-cells.context-menu.reverse-selected-cells.name", "Reverse Selected Cells")
            , LOCTEXT("timeline-cells.context-menu.reverse-selected-cells.tooltip", "Reverse the order of the selected cells")
            , FSlateIcon()
            , FUIAction( FExecuteAction::CreateSP(this, &SOdysseyAnimationLayerImageTimeline::ReverseSelectedCells)
                       , FCanExecuteAction::CreateSP(this, &SOdysseyAnimationLayerImageTimeline::CanReverseSelectedCells))
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


void
SOdysseyAnimationLayerImageTimeline::ReverseSelectedCells()
{
    TSharedRef<FOdysseyLayerCellSelection> cellSelection = mLayer->GetLayerStack()->GetCellSelection();
    TArray<UOdysseyLayerCell*> selectedCells = cellSelection.Get().GetSelectedCells();

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.reverse-selected-cells", "Reverse Selected Cells"));
#endif
    mLayer->ReverseCells( selectedCells );
}

bool
SOdysseyAnimationLayerImageTimeline::CanReverseSelectedCells()
{
    TSharedRef<FOdysseyLayerCellSelection> cellSelection = mLayer->GetLayerStack()->GetCellSelection();
    TArray<UOdysseyLayerCell*> selectedCells = cellSelection.Get().GetSelectedCells();

    return UOdysseyAnimationLayer::AreCellsContiguous( selectedCells ) && ( selectedCells.Num() > 1 ) ? true
                                                                                                      : false;
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
SOdysseyAnimationLayerImageTimeline::BuildCellsMarksSubMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.AddMenuEntry(
        LOCTEXT("timeline-cells.context-menu.cell-mark.reset.name", "Remove"),
        LOCTEXT("timeline-cells.context-menu.cell-mark.reset.tooltip", "Removes the any mark from selected cells"),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::RemoveCellMark),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanRemoveCellMark)
        )
    );

    iMenuBuilder.AddSeparator();

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    for (int i = 0; i < settings->AnimationCellsMarks.Num(); i++)
    {
        iMenuBuilder.AddMenuEntry(
            FUIAction(
                FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::SetCellMark, i),
                FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanSetCellMark),
                FIsActionChecked::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::IsCellMarkChecked, i)
            ),
            CreateCellMarkMenuWidget(i),
            NAME_None,
            TAttribute<FText>(),
            EUserInterfaceActionType::RadioButton
        );
    }
}

void
SOdysseyAnimationLayerImageTimeline::RemoveCellMark()
{
    if (!mLayer->IsEditable())
        return;

    TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.set-mark", "Set cell mark"));
#endif
    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    for (UOdysseyLayerCell* selectedCell : selectedCells)
    {
        selectedCell->SetMark(INDEX_NONE);
    }
}

bool
SOdysseyAnimationLayerImageTimeline::CanRemoveCellMark() const
{
    if (!mLayer->IsEditable())
        return false;

    TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

void
SOdysseyAnimationLayerImageTimeline::SetCellMark( int iMarkId )
{
    if (!mLayer->IsEditable())
        return;

    TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.set-mark", "Set cell mark"));
#endif
    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    for (UOdysseyLayerCell* selectedCell : selectedCells)
    {
        selectedCell->SetMark(iMarkId);
    }
}

bool
SOdysseyAnimationLayerImageTimeline::CanSetCellMark() const
{
    if (!mLayer->IsEditable())
        return false;

    TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
SOdysseyAnimationLayerImageTimeline::IsCellMarkChecked(int iMarkId) const
{
    TArray<UOdysseyLayerCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    for (UOdysseyLayerCell* selectedCell : selectedCells)
    {
        if (selectedCell->GetMark() != iMarkId)
            return false;
    }

    return true;
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::CreateCellMarkMenuWidget(int iMarkId)
{
    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[iMarkId];
    const FSlateBrush* icon = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
    switch(markSettings.Symbol)
    {
        case EOdysseyAnimationCellMarkSymbol::Triangle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Triangle"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledTriangle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Triangle"); break;
        case EOdysseyAnimationCellMarkSymbol::Circle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Circle"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledCircle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Circle"); break;
        case EOdysseyAnimationCellMarkSymbol::Diamond: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Diamond"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledDiamond: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Diamond"); break;
        case EOdysseyAnimationCellMarkSymbol::Star: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Star"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledStar: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Star"); break;
        case EOdysseyAnimationCellMarkSymbol::Cross: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Cross"); break;
        case EOdysseyAnimationCellMarkSymbol::Checkmark: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Checkmark"); break;
    }
    FLinearColor iconColor = markSettings.Color;
    FText name = FText::FromName(markSettings.Name);

    return SNew(SHorizontalBox)
    + SHorizontalBox::Slot()
    .Padding(FMargin(0, 0, 4, 0))
    .AutoWidth()
    [
        SNew(SImage)
        .Image(icon)
        .ColorAndOpacity(iconColor)
    ]
    + SHorizontalBox::Slot()
    .AutoWidth()
    [
        SNew(STextBlock)
        .Text(name)
    ];
}


#undef LOCTEXT_NAMESPACE
