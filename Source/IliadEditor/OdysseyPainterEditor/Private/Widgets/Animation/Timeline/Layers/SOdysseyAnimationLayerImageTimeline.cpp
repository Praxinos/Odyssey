// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerImageTimeline.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLightTable.h"
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
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineScrollBox.h"
#include "TimelineTools/OdysseyAnimationTimelineTools.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "LayerStack/Cells/OdysseyAnimationCellSelection.h"
#include "ScopedTransaction.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageTimeline::~SOdysseyAnimationLayerImageTimeline()
{
}

SOdysseyAnimationLayerImageTimeline::SOdysseyAnimationLayerImageTimeline()
    : mIsDraggingOver(false)
    , mDragState(kDrag_None)
    , mDragPosition(0)
    , mAnimationTimelineCellsShortcuts(nullptr)
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
    mTimelinePosition = iArgs._TimelinePosition;
    mOnActivateOutOfPegs = iArgs._OnActivateOutOfPegs;
    mOnInactivateOutOfPegs = iArgs._OnInactivateOutOfPegs;
    mOnIsOutOfPegsChecked = iArgs._OnIsOutOfPegsChecked;
    mEditor = iArgs._PainterEditor;

    SOdysseyAnimationLayerTimeline::Construct(SOdysseyAnimationLayerTimeline::FArguments(), iOwnerTableView, iLayer);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::GenerateWidgetForColumn( const FName& InColumnName )
{
    TSharedPtr<SWidget> widget = SOdysseyLayerRowBase::GenerateWidgetForColumn( InColumnName );
    return SNew(SOdysseyAnimationTimelineScrollBox)
        .TimelinePosition(mTimelinePosition)
        + SOdysseyAnimationTimelineScrollBox::Slot()
        [
            widget.ToSharedRef()
        ];
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
        return GenerateLightTableRowTimelineWidget();
    }
    if (iRow == "OutOfPegs")
    {
        return GenerateOutOfPegsRowTimelineWidget();
    }

    return SOdysseyAnimationLayerTimeline::GenerateWidget( iRow, iColumn );
}

FOptionalSize
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
            [this]() -> TArray<UOdysseyAnimationCell*>
            {
                return mLayer->GetCells();
            }
        )
        .TimelinePosition(mTimelinePosition)
        .IsEnabled_Lambda([this](){ return !mLayer->IsLockedRecursively();})
        .OnCreateCellWidget(this, &SOdysseyAnimationLayerImageTimeline::OnGenerateCellWidget)
        .ShowHandles(this, &SOdysseyAnimationLayerImageTimeline::GetShowCellsHandles);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::GenerateLightTableRowTimelineWidget()
{
    return SNew(SOdysseyAnimationTimelineLightTable, mLayer)
        .TimelinePosition(mTimelinePosition);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::GenerateOutOfPegsRowTimelineWidget()
{
    return SNew(SOdysseyAnimationTimelineOutOfPegs, mLayer)
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

    return mTool->OnMouseButtonDown(params);
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
    if (iEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        int frame = (int)MousePositionToFrame(iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition()).X);
        if (frame < 0)
            return FReply::Unhandled();

        //On Right click, select cell if none are selected yet
        UOdysseyAnimationCell* cell = mLayer->GetCellAtFrame(frame);
        if (!cell)
            return FReply::Unhandled();

        const TArray<UOdysseyAnimationCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
        if (selectedCells.IsEmpty() || !selectedCells.Contains(cell))
            mLayer->GetLayerStack()->GetCellSelection()->SetSelectedCells({cell});

        TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
        MapActions(commandList, frame);
        FMenuBuilder menuBuilder(true, commandList, ExtendContextMenu());
        BuildContextMenu(menuBuilder);

        TSharedRef<SWidget> menuContents = menuBuilder.MakeWidget();
        FWidgetPath widgetPath = iEvent.GetEventPath() != nullptr ? *iEvent.GetEventPath() : FWidgetPath();
        FSlateApplication::Get().PushMenu(AsShared(), widgetPath, menuContents, iEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
        return FReply::Handled();
    }

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

    UOdysseyAnimationCell* cell = mLayer->GetCellAtFrame((int)frame);
    if (!cell)
        return FReply::Unhandled();

    int cellStartFrame = cell->GetFrameRange().GetLowerBoundValue();
    float position = (frame - cellStartFrame) / cell->Exposure;
    if (position < 0.5f)
    {
        mDragPosition = cellStartFrame;
    }
    else
    {
        mDragPosition = cellStartFrame + cell->Exposure;
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
        const TArray<UOdysseyAnimationCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
        UOdysseyAnimationCell* nextCell = layer->GetCellAtFrame(mDragPosition);
        UOdysseyAnimationCell* previousCell = layer->GetCellAtFrame(mDragPosition - 1);

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
SOdysseyAnimationLayerImageTimeline::ExtendContextMenu()
{
    return nullptr;
}

void
SOdysseyAnimationLayerImageTimeline::BuildContextMenu(FMenuBuilder& iMenuBuilder)
{
    //iMenuBuilder.BeginSection("Selection", LOCTEXT("timeline-cells.context-menu.selection-section.name", "Selection"));
    iMenuBuilder.AddWidget(
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
    //iMenuBuilder.EndSection();

    iMenuBuilder.BeginSection("Selection", LOCTEXT("timeline-cells.context-menu.selection-section.name", "Selection"));
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().SelectAll);
    iMenuBuilder.EndSection();

    iMenuBuilder.BeginSection("Common", LOCTEXT("timeline-cells.context-menu.common-section.name", "Common"));
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Cut);
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Paste);
        iMenuBuilder.AddSeparator("");
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
    iMenuBuilder.EndSection();

    iMenuBuilder.BeginSection("Cells", LOCTEXT("timeline-cells.context-menu.cells-section.name", "Cells"));
        iMenuBuilder.AddMenuEntry(FOdysseyPainterEditorAnimationCommands::Get().ConvertToStaggerCell, TEXT("ConvertToStagger"));
        iMenuBuilder.AddMenuEntry(FOdysseyPainterEditorAnimationCommands::Get().ConvertToReferenceCells);
        iMenuBuilder.AddMenuEntry(
            FOdysseyPainterEditorAnimationCommands::Get().SetCellExposure,
            NAME_None,
            LOCTEXT("timeline-cells.context-menu.set-selected-cells-exposure.name", "Set Exposure")
        );
        iMenuBuilder.AddSubMenu(
            LOCTEXT("timeline-cells.context-menu.cell-mark.name", "Mark"),
            LOCTEXT("timeline-cells.context-menu.cell-mark.tooltip", "Set a mark on the selected cells"),
            FNewMenuDelegate::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::BuildCellsMarksSubMenu)
        );
    iMenuBuilder.EndSection();
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
SOdysseyAnimationLayerImageTimeline::MapActions(TSharedPtr<FUICommandList> iCommandList, int iFrame)
{
    mAnimationTimelineCellsShortcuts = MakeShared<FOdysseyAnimationTimelineCellsShortcuts>(mLayer->GetLayerStack());
    mAnimationTimelineCellsShortcuts->MapActionsToCommandList(iCommandList.ToSharedRef());

    mAnimationTimelineCellImageStaggerShortcuts = MakeShared<FOdysseyAnimationTimelineCellImageStaggerShortcuts>(mLayer->GetLayerStack());
    mAnimationTimelineCellImageStaggerShortcuts->MapActionsToCommandList(iCommandList.ToSharedRef());
}

EVisibility
SOdysseyAnimationLayerImageTimeline::GetLightTableVisibility() const
{
    return mLayer->Lighttable.bIsActivated && mLayer->DisplayOptions ? EVisibility::Visible : EVisibility::Collapsed;
}

bool
SOdysseyAnimationLayerImageTimeline::GetShowCellsHandles() const
{
    if (mLayer->IsLockedRecursively())
        return false;

    return mLayer->DisplayOptions;;
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

void
SOdysseyAnimationLayerImageTimeline::RemoveCellMark()
{
    if (mLayer->IsLockedRecursively())
        return;

    TArray<UOdysseyAnimationCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.set-mark", "Set cell mark"));
#endif
    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(selectedCells[0]->GetAnimation());
    currentFrameMutator.Set(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    currentFrameMutator.Commit();

    for (UOdysseyAnimationCell* selectedCell : selectedCells)
    {
        FOdysseyObjectEditorUtils::SetPropertyValue(selectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Mark), INDEX_NONE);
    }
}

bool
SOdysseyAnimationLayerImageTimeline::CanRemoveCellMark() const
{
    if (mLayer->IsLockedRecursively())
        return false;

    TArray<UOdysseyAnimationCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

void
SOdysseyAnimationLayerImageTimeline::SetCellMark( int iMarkId )
{
    if (mLayer->IsLockedRecursively())
        return;

    TArray<UOdysseyAnimationCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.set-mark", "Set cell mark"));
#endif
    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(selectedCells[0]->GetAnimation());
    currentFrameMutator.Set(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    currentFrameMutator.Commit();

    for (UOdysseyAnimationCell* selectedCell : selectedCells)
    {
        FOdysseyObjectEditorUtils::SetPropertyValue(selectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Mark), iMarkId);
    }
}

bool
SOdysseyAnimationLayerImageTimeline::CanSetCellMark() const
{
    if (mLayer->IsLockedRecursively())
        return false;

    TArray<UOdysseyAnimationCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
SOdysseyAnimationLayerImageTimeline::IsCellMarkChecked(int iMarkId) const
{
    if (mLayer->IsLockedRecursively())
        return false;

    TArray<UOdysseyAnimationCell*> selectedCells = mLayer->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    for (UOdysseyAnimationCell* selectedCell : selectedCells)
    {
        if (selectedCell->Mark != iMarkId)
            return false;
    }

    return true;
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
