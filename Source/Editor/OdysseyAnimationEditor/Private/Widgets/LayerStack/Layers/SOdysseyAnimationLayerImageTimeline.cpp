// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerImageTimeline.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTable.h"
#include "Widgets/LayerStack/Cells/SOdysseyAnimationCells.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineTool.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "DragDropOperations/OdysseyAnimationCellsDragDropOperation.h"
#include "Framework/Commands/GenericCommands.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "AnimationEditor/OdysseyAnimationEditorProjectSettings.h"
#include "OdysseyStyleSet.h"
#include "Shortcuts/Timeline/OdysseyAnimationTimelineCellsShortcuts.h"
#include "OdysseyAnimationCurrentFrameMutator.h"

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
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayer* iLayer
)
{
    ensure(iLayer);

    mExtension = iExtension;
    mLayer = iLayer;
    mDisplayOptions = iArgs._DisplayOptions;

    TAttribute<FMargin> cellsPadding = TAttribute<FMargin>::CreateLambda(
        [this]()
        {
            return /*!DisplayOptions() ? FMargin(0.f, 5.f, 0.f, 5.f) :*/ FMargin(0);
        }
    );
    
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(cellsPadding)
        [
            SNew(SOdysseyAnimationCells, mExtension, mLayer)
            .IsEnabled_Lambda([this](){ return !mLayer->IsLockedRecursively();})
            .OnCreateCellWidget(this, &SOdysseyAnimationLayerImageTimeline::OnGenerateCellWidget)
            .ShowHandles(this, &SOdysseyAnimationLayerImageTimeline::GetShowCellsHandles)
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
    return mExtension->Timeline()->GetTool()->OnMouseButtonDown(params);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::Layer,
        mLayer
    };
    return mExtension->Timeline()->GetTool()->OnMouseMove(params);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::Layer,
        mLayer
    };
    return mExtension->Timeline()->GetTool()->OnDragDetected(params);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	if (iEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        int frame = mExtension->Timeline()->GetFrameIndexAtMousePosition(iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition()).X);
        if (frame == INDEX_NONE)
            return FReply::Unhandled();

        //On Right click, select cell if none are selected yet
        UOdysseyAnimationCell* cell = mLayer->GetCellAtFrame(frame);
        if (!cell)
            return FReply::Unhandled();

        const TArray<UOdysseyAnimationCell*> selectedCells = mExtension->Timeline()->GetSelectedCells();
        if (selectedCells.IsEmpty() || !selectedCells.Contains(cell))
            mExtension->Timeline()->SetSelectedCells({cell});

        TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
        MapActions(commandList, frame);
		FMenuBuilder menuBuilder(true, commandList, ExtendContextMenu());
		BuildContextMenu(menuBuilder);

		TSharedRef<SWidget> menuContents = menuBuilder.MakeWidget();
		FWidgetPath widgetPath = iEvent.GetEventPath() != nullptr ? *iEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), widgetPath, menuContents, iEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
    	return FReply::Handled();
	}
    
    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::Layer,
        mLayer
    };
    mExtension->Timeline()->GetTool()->OnMouseButtonUp(params);
	return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
    return mExtension->Timeline()->GetTool()->OnKeyDown(iKeyEvent);
}

FReply
SOdysseyAnimationLayerImageTimeline::OnKeyUp( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
    return mExtension->Timeline()->GetTool()->OnKeyUp(iKeyEvent);
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

    float timelineOffset = mExtension->Timeline()->GetOffset();
    float posX = iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition()).X;
    float frameWidth = mExtension->Timeline()->GetFrameWidth();
    float frame = (posX / frameWidth + timelineOffset);

    UOdysseyAnimationCell* cell = mLayer->GetCellAtFrame((int)frame);
    if (!cell)
        return FReply::Unhandled();

    int cellStartFrame = cell->GetFrameRange().GetLowerBoundValue();
    float position = (frame - cellStartFrame) / cell->Length;
    if (position < 0.5f)
    {
        mDragPosition = cellStartFrame;
    }
    else
    {
        mDragPosition = cellStartFrame + cell->Length;
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
        const TArray<UOdysseyAnimationCell*> selectedCells = mExtension->Timeline()->GetSelectedCells();
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
        iMenuBuilder.AddMenuEntry(FOdysseyAnimationEditorCommands::Get().ConvertToStaggerCell, TEXT("ConvertToStagger"));
        iMenuBuilder.AddMenuEntry(
            FOdysseyAnimationEditorCommands::Get().SetCellLength,
            NAME_None,
            LOCTEXT("timeline-cells.context-menu.set-selected-cells-length.name", "Set Length")
        );
        iMenuBuilder.AddSubMenu(
            LOCTEXT("timeline-cells.context-menu.cell-mark.name", "Mark"),
            LOCTEXT("timeline-cells.context-menu.cell-mark.tooltip", "Set a mark on the selected cells"),
            FNewMenuDelegate::CreateRaw(this, &SOdysseyAnimationLayerImageTimeline::BuildCellsMarksSubMenu)
        );
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

TSharedRef<SWidget>
SOdysseyAnimationLayerImageTimeline::CreateCellMarkMenuWidget(int iMarkId)
{
    UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();
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

    UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();
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
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PostBehaviour), iBehaviour);
}

bool
SOdysseyAnimationLayerImageTimeline::IsPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return mLayer->PostBehaviour == iBehaviour;
}

bool
SOdysseyAnimationLayerImageTimeline::CanSetPostBehaviour() const
{
    return !mLayer->IsLockedRecursively();
}

void
SOdysseyAnimationLayerImageTimeline::SetPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.set-pre-behaviour", "Set Pre Behaviour"));
#endif
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PreBehaviour), iBehaviour);
}

bool
SOdysseyAnimationLayerImageTimeline::IsPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return mLayer->PreBehaviour == iBehaviour;
}

bool
SOdysseyAnimationLayerImageTimeline::CanSetPreBehaviour() const
{
    return !mLayer->IsLockedRecursively();
}

void
SOdysseyAnimationLayerImageTimeline::MapActions(TSharedPtr<FUICommandList> iCommandList, int iFrame)
{
    mAnimationTimelineCellsShortcuts = MakeShared<FOdysseyAnimationTimelineCellsShortcuts>(mLayer->GetLayerStack(), mExtension);
    mAnimationTimelineCellsShortcuts->MapActionsToCommandList(iCommandList.ToSharedRef());
}

EVisibility
SOdysseyAnimationLayerImageTimeline::GetLightTableVisibility() const
{
    return mLayer->Lighttable.bIsActivated && mDisplayOptions.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}

bool
SOdysseyAnimationLayerImageTimeline::DisplayOptions() const
{
    return mDisplayOptions.Get();
}

bool
SOdysseyAnimationLayerImageTimeline::GetShowCellsHandles() const
{
    return mDisplayOptions.Get();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnContextMenuMinusButtonClicked()
{
    mAnimationTimelineCellsShortcuts->Action_DecreaseCellLength();
    return FReply::Handled();
}

FReply
SOdysseyAnimationLayerImageTimeline::OnContextMenuPlusButtonClicked()
{
    mAnimationTimelineCellsShortcuts->Action_IncreaseCellLength();
    return FReply::Handled();
}

void
SOdysseyAnimationLayerImageTimeline::RemoveCellMark()
{
    if (mLayer->IsLockedRecursively())
        return;

    TArray<UOdysseyAnimationCell*> selectedCells = mExtension->Timeline()->GetSelectedCells();
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

    TArray<UOdysseyAnimationCell*> selectedCells = mExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

void
SOdysseyAnimationLayerImageTimeline::SetCellMark( int iMarkId )
{
    if (mLayer->IsLockedRecursively())
        return;

    TArray<UOdysseyAnimationCell*> selectedCells = mExtension->Timeline()->GetSelectedCells();
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

    TArray<UOdysseyAnimationCell*> selectedCells = mExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
SOdysseyAnimationLayerImageTimeline::IsCellMarkChecked(int iMarkId) const
{
    if (mLayer->IsLockedRecursively())
        return false;

    TArray<UOdysseyAnimationCell*> selectedCells = mExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    for (UOdysseyAnimationCell* selectedCell : selectedCells)
    {
        if (selectedCell->Mark != iMarkId)
            return false;
    }

    return true;
}

#undef LOCTEXT_NAMESPACE
