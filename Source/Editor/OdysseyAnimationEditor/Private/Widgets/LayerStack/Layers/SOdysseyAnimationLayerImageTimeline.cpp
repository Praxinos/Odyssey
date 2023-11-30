// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerImageTimeline.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTable.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageTimeline::~SOdysseyAnimationLayerImageTimeline()
{
}

SOdysseyAnimationLayerImageTimeline::SOdysseyAnimationLayerImageTimeline()
    : mIsDraggingOver(false)
    , mDragState(kDrag_None)
    , mDragPosition(0)
    , mIsSelectingFrames(false)
	, mInteractiveFrameSelection(FInt32Range::Empty())
{
}

void
SOdysseyAnimationLayerImageTimeline::Construct(
    const FArguments& iArgs, 
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayer* iLayer
)
{
    ensure(iLayer);

    mExtension = iExtension;
    mLayer = iLayer;
    mIsCollapsed = iArgs._IsCollapsed;

    TAttribute<FMargin> cellsPadding = TAttribute<FMargin>::CreateLambda(
        [this]()
        {
            return IsCollapsed() ? FMargin(0.f, 5.f, 0.f, 5.f) : FMargin(0);
        }
    );
    
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        [
            SNew(SOdysseyAnimationTimelineFrameSelection, mExtension)
            .SelectedFrames(this, &SOdysseyAnimationLayerImageTimeline::GetSelectedFrames)
            .OnSelectionDragged(this, &SOdysseyAnimationLayerImageTimeline::OnFramesSelectionDragged)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .Padding(cellsPadding)
                [
                    SNew(SOdysseyAnimationCells, mExtension, mLayer, mLayer->GetCellsContainer())
                    .OnCreateCell(this, &SOdysseyAnimationLayerImageTimeline::OnCreateCell)
                    .OnCreateCellWidget(this, &SOdysseyAnimationLayerImageTimeline::OnGenerateCellWidget)
                    .ShowHandles(this, &SOdysseyAnimationLayerImageTimeline::GetShowCellsHandles)
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SOdysseyAnimationTimelineFrameSelector, mExtension)
                    .Visibility(this, &SOdysseyAnimationLayerImageTimeline::GetFrameSelectorVisibility)
                    .SelectableFrames(this, &SOdysseyAnimationLayerImageTimeline::GetSelectableFrames)
                    .OnSelectionStarted(this, &SOdysseyAnimationLayerImageTimeline::OnFramesSelectionStarted)
                    .OnSelectionEnded(this, &SOdysseyAnimationLayerImageTimeline::OnFramesSelectionEnded)
                    .OnSelectionChanged(this, &SOdysseyAnimationLayerImageTimeline::OnFramesSelectionChanged)
                ]
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationTimelineLightTable, mLayer, mExtension)
            .Visibility(this, &SOdysseyAnimationLayerImageTimeline::GetLightTableVisibility)
        ]
    ];
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	if (iEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        int frame = mExtension->Timeline()->GetFrameIndexAtMousePosition(iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition()).X);
        if (frame == INDEX_NONE)
            return FReply::Unhandled();

        TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
        MapActions(commandList, frame);
		FMenuBuilder menuBuilder(true, commandList);
		BuildContextMenu(menuBuilder);

		TSharedRef<SWidget> menuContents = menuBuilder.MakeWidget();
		FWidgetPath widgetPath = iEvent.GetEventPath() != nullptr ? *iEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), widgetPath, menuContents, iEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
    	return FReply::Handled();
	}
    else
    {
        mExtension->Timeline()->SetSelectedFrames(FInt32Range::Empty());
    }
	return FReply::Unhandled();
}

int32
SOdysseyAnimationLayerImageTimeline::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Draw a current frame
	LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	++LayerId;

	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );

	const float height = AllottedGeometry.GetLocalSize().Y;
	const float width = AllottedGeometry.GetLocalSize().X;
	float offset = mExtension->Timeline()->GetOffset();
	const float frameSize = mExtension->Timeline()->GetFrameWidth();

	if(mIsDraggingOver && mDragState != kDrag_None)
	{
        //Dragging Zone
        FLinearColor lineColor(0.2f, 0.2f, 1.f);
		float dragPos = (mDragPosition - offset) * frameSize;

        //Dragging Bar
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(FVector2D(dragPos, 0.f), FVector2D(3.f, height)),
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
                AllottedGeometry.ToPaintGeometry(FVector2D(dragPos + 5.f,  5.f + plusSize), FVector2D( 3 * plusSize, plusSize)),
                GenericBrush,
                ESlateDrawEffect::None,
                lineColor
            );

            //Plus
            FSlateDrawElement::MakeBox(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry(FVector2D(dragPos + 5.f + plusSize,  5.f), FVector2D(plusSize, 3 * plusSize)),
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

    int timelineOffset = mExtension->Timeline()->GetOffset();
    float posX = iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition()).X;
    float frameWidth = mExtension->Timeline()->GetFrameWidth();
    float frame = (int)(posX / frameWidth + timelineOffset + 0.5f);

    mDragPosition = frame;

    /*
    if (FSlateApplication::Get().GetModifierKeys().IsControlDown())
        mDragState = kDrag_Copy;
    else
        mDragState = kDrag_Move;
    */
   mDragState = kDrag_Copy; //For now we can only copy frames, we will be able to move them when layers will have holes

    //Check if the copy or move is actually allowed
    UOdysseyAnimationLayer* layer = operation->GetLayer();
    if (layer == mLayer)
    {
        FInt32Range selectedFrames = mExtension->Timeline()->GetSelectedFrames();
        if (mDragState == kDrag_Copy && frame > selectedFrames.GetLowerBoundValue() && frame <= selectedFrames.GetUpperBoundValue())
        {
            //Trying to copy cells inside the current layer selection
            //This is not allowed
            mDragState = kDrag_None;
            return FReply::Handled();
        }
        else if (mDragState == kDrag_Move && frame >= selectedFrames.GetLowerBoundValue() && frame <= selectedFrames.GetUpperBoundValue() + 1)
        {
            //Trying to move cells inside the current layer selection
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

void
SOdysseyAnimationLayerImageTimeline::SelectAllFrames()
{
    FInt32Range frameRange = mLayer->GetFrameRange();
    mExtension->Timeline()->SetSelectedFrames(frameRange);
}

void
SOdysseyAnimationLayerImageTimeline::CopyFrames()
{
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(mLayer, mExtension->Timeline()->GetSelectedFrames());
    FOdysseyClipboard::Get().SetData(clipboardData);
}

void
SOdysseyAnimationLayerImageTimeline::CutFrames()
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.cut", "Cut Frames"));
#endif
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(mLayer, mExtension->Timeline()->GetSelectedFrames());
    FOdysseyClipboard::Get().SetData(clipboardData);
    DeleteSelectedFrames();
}

void
SOdysseyAnimationLayerImageTimeline::PasteFrames()
{
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = FOdysseyClipboard::Get().GetData<FOdysseyAnimationCellClipboardData>();
    if (!clipboardData)
        return;

    if (!clipboardData->CanPaste(mLayer))
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.paste", "Paste Frames"));
#endif

    clipboardData->Paste(mLayer, mExtension->Animation()->CurrentFrame);
}

void
SOdysseyAnimationLayerImageTimeline::DeleteSelectedFrames()
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.delete", "Remove Frames"));
#endif

    FOdysseyAnimationCellsMutator mutator(mLayer, mLayer->GetCellsContainer());

    bool isLowerClosed = mExtension->Timeline()->GetSelectedFrames().GetLowerBound().IsClosed();
    bool isUpperClosed = mExtension->Timeline()->GetSelectedFrames().GetUpperBound().IsClosed();

    if ( !isLowerClosed || !isUpperClosed )
    {
        mutator.RemoveFrame(mExtension->Animation()->CurrentFrame);
    }
    else
    {
        mutator.RemoveFrameRange(mExtension->Timeline()->GetSelectedFrames());
    }

    mutator.Commit();
}

void
SOdysseyAnimationLayerImageTimeline::StaggerCell( int iFrame )
{
    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();
    if (!cellsContainer)
        return;

    int cellFrame = cellsContainer->GetCellFrameAtFrame(iFrame);
    if (cellFrame == INDEX_NONE || cellFrame == 0)
        return;

    int cellIndex = cellsContainer->GetCellIndexAtFrame(iFrame);
    if (cellIndex == INDEX_NONE)
        return;

    TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCells()[cellIndex];
    if (!cell || cell->GetType() == FOdysseyAnimationCellImageStagger::StaticType())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.create-stagger-cell", "Stagger Cell"));
#endif
    
    int cellStaggerLength = cell->GetLength() - cellFrame;
    TSharedPtr<FOdysseyAnimationCellImageStagger> cellStagger = FOdysseyAnimationCellImageStagger::Create(mLayer, cellStaggerLength);
    
    FOdysseyAnimationCellsMutator mutator(mLayer, cellsContainer);
    mutator.SetLength( cellIndex, cellFrame );
    mutator.Add({cellStagger}, cellIndex + 1);
    mutator.Commit();
}

bool
SOdysseyAnimationLayerImageTimeline::CanStaggerCell( int iFrame ) const
{
    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    int cellFrame = cellsContainer->GetCellFrameAtFrame(iFrame);
    if (cellFrame == INDEX_NONE || cellFrame == 0)
        return false;

    int cellIndex = cellsContainer->GetCellIndexAtFrame(iFrame);
    if (cellIndex == INDEX_NONE)
        return false;

    TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCells()[cellIndex];
    if (!cell || cell->GetType() == FOdysseyAnimationCellImageStagger::StaticType())
        return false;

    return true;
}

FInt32Range
SOdysseyAnimationLayerImageTimeline::GetSelectableFrames() const
{
    return mExtension->Timeline()->GetSelectableFrames();
}

FInt32Range
SOdysseyAnimationLayerImageTimeline::GetSelectedFrames() const
{
    if (mIsSelectingFrames)
        return mInteractiveFrameSelection;

    bool isCurrentLayer = mLayer->GetLayerStack()->CurrentLayer == mLayer;
	return isCurrentLayer ? mExtension->Timeline()->GetSelectedFrames() : FInt32Range::Empty();
}

void
SOdysseyAnimationLayerImageTimeline::OnFramesSelectionChanged(FInt32Range iSelectedFrames)
{
	mInteractiveFrameSelection = iSelectedFrames;
}

void
SOdysseyAnimationLayerImageTimeline::OnFramesSelectionStarted(int iFrame)
{
    mIsSelectingFrames = true;
	mInteractiveFrameSelection = FInt32Range::Empty();
}

void
SOdysseyAnimationLayerImageTimeline::OnFramesSelectionEnded(int iFrame)
{
    mIsSelectingFrames = false;
    mExtension->Timeline()->SetSelectedFrames(mInteractiveFrameSelection);

    if (mExtension->Timeline()->GetSelectedFrames().IsEmpty())
        return; //Only change currentframe if the selection has been set

    FInt32Range frameRange = mLayer->GetFrameRange();
    int frame = FMath::Clamp(iFrame, frameRange.GetLowerBoundValue(), frameRange.GetUpperBoundValue());

    FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Animation(), "CurrentFrame", frame);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnFramesSelectionDragged()
{
    TSharedRef<FOdysseyAnimationCellsDragDropOperation> operation = FOdysseyAnimationCellsDragDropOperation::Create(mLayer, mExtension->Timeline()->GetSelectedFrames());
    return FReply::Handled().BeginDragDrop(operation);
}

void
SOdysseyAnimationLayerImageTimeline::BuildContextMenu(FMenuBuilder& iMenuBuilder)
{   
    iMenuBuilder.BeginSection("Selection", LOCTEXT("timeline-cells.context-menu.selection-section.name", "Selection"));
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().SelectAll);
    iMenuBuilder.EndSection();

    iMenuBuilder.BeginSection("Common", LOCTEXT("timeline-cells.context-menu.common-section.name", "Common"));
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
        iMenuBuilder.AddSeparator("");
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Cut);
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Paste);
        iMenuBuilder.AddSeparator("");
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
    iMenuBuilder.EndSection();

    iMenuBuilder.BeginSection("Cells", LOCTEXT("timeline-cells.context-menu.cells-section.name", "Cells"));
        iMenuBuilder.AddMenuEntry(FOdysseyAnimationEditorCommands::Get().StaggerCell);
    iMenuBuilder.EndSection();

    iMenuBuilder.BeginSection("Layer", LOCTEXT("timeline-cells.context-menu.layer-section.name", "Layer"));
        iMenuBuilder.AddSubMenu(
            LOCTEXT("timeline-cells.context-menu.pre-behaviour-submenu.name", "Pre Behaviour"),
            LOCTEXT("timeline-cells.context-menu.pre-behaviour-submenu.tooltip", "Set Layer's Pre Behaviour"),
            FNewMenuDelegate::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::BuildPreBehaviourSubMenu)
        );
        iMenuBuilder.AddSubMenu(
            LOCTEXT("timeline-cells.context-menu.post-behaviour-submenu.name", "Post Behaviour"),
            LOCTEXT("timeline-cells.context-menu.post-behaviour-submenu.tooltip", "Set Layer's Post Behaviour"),
            FNewMenuDelegate::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::BuildPostBehaviourSubMenu)
        );
    iMenuBuilder.EndSection();
}

void
SOdysseyAnimationLayerImageTimeline::BuildPreBehaviourSubMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.AddMenuEntry(
        LOCTEXT("timeline-cells.context-menu.pre-behaviour.none", "None"),
        TAttribute<FText>(),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanSetPreBehaviour),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );
    iMenuBuilder.AddMenuEntry(
        LOCTEXT("timeline-cells.context-menu.pre-behaviour.hold", "Hold"),
        TAttribute<FText>(),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanSetPreBehaviour),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );
    iMenuBuilder.AddMenuEntry(
        LOCTEXT("timeline-cells.context-menu.pre-behaviour.loop", "Loop"),
        TAttribute<FText>(),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanSetPreBehaviour),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );
    iMenuBuilder.AddMenuEntry(
        LOCTEXT("timeline-cells.context-menu.pre-behaviour.pingpong", "PingPong"),
        TAttribute<FText>(),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanSetPreBehaviour),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );
}

void
SOdysseyAnimationLayerImageTimeline::BuildPostBehaviourSubMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.AddMenuEntry(
        LOCTEXT("timeline-cells.context-menu.post-behaviour.none", "None"),
        TAttribute<FText>(),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanSetPostBehaviour),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );
    iMenuBuilder.AddMenuEntry(
        LOCTEXT("timeline-cells.context-menu.post-behaviour.hold", "Hold"),
        TAttribute<FText>(),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanSetPostBehaviour),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );
    iMenuBuilder.AddMenuEntry(
        LOCTEXT("timeline-cells.context-menu.post-behaviour.loop", "Loop"),
        TAttribute<FText>(),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanSetPostBehaviour),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );
    iMenuBuilder.AddMenuEntry(
        LOCTEXT("timeline-cells.context-menu.post-behaviour.pingpong", "PingPong"),
        TAttribute<FText>(),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanSetPostBehaviour),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );
}

void
SOdysseyAnimationLayerImageTimeline::SetPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.set-post-behaviour", "Set Post Behaviour"));
#endif
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, "PostBehaviour", iBehaviour);
}

bool
SOdysseyAnimationLayerImageTimeline::IsPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return mLayer->PostBehaviour == iBehaviour;
}

bool
SOdysseyAnimationLayerImageTimeline::CanSetPostBehaviour() const
{
    return !mLayer->IsLocked;
}

void
SOdysseyAnimationLayerImageTimeline::SetPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.set-pre-behaviour", "Set Pre Behaviour"));
#endif
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, "PreBehaviour", iBehaviour);
}

bool
SOdysseyAnimationLayerImageTimeline::IsPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return mLayer->PreBehaviour == iBehaviour;
}

bool
SOdysseyAnimationLayerImageTimeline::CanSetPreBehaviour() const
{
    return !mLayer->IsLocked;
}

void
SOdysseyAnimationLayerImageTimeline::MapActions(TSharedPtr<FUICommandList> iCommandList, int iFrame)
{
	iCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::SelectAllFrames)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::DeleteSelectedFrames)
    );

	iCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CopyFrames)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CutFrames)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::PasteFrames)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().StaggerCell,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::StaggerCell, iFrame),
        FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::CanStaggerCell, iFrame)
    );
}

EVisibility
SOdysseyAnimationLayerImageTimeline::GetLightTableVisibility() const
{
    return mLayer->GetIsLightTableActivated() && !mIsCollapsed.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}

bool
SOdysseyAnimationLayerImageTimeline::IsCollapsed() const
{
    return mIsCollapsed.Get();
}

EVisibility
SOdysseyAnimationLayerImageTimeline::GetFrameSelectorVisibility() const
{
    return mIsCollapsed.Get() ? EVisibility::Collapsed : EVisibility::Visible;
}

bool
SOdysseyAnimationLayerImageTimeline::GetShowCellsHandles() const
{
    return !mIsCollapsed.Get();
}

#undef LOCTEXT_NAMESPACE
