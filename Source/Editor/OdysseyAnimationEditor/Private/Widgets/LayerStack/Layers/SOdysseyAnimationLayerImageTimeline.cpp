// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerImageTimeline.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageTimeline"

SOdysseyAnimationLayerImageTimeline::~SOdysseyAnimationLayerImageTimeline()
{
}

SOdysseyAnimationLayerImageTimeline::SOdysseyAnimationLayerImageTimeline()
    : mCommandList(MakeShared<FUICommandList>())
{
    MapActions(mCommandList);
}

void
SOdysseyAnimationLayerImageTimeline::Construct(
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayer* iLayer
)
{
    ensure(iLayer);

    mExtension = iExtension;
    mLayer = iLayer;
    
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationCells, mExtension, mLayer, mLayer->GetCellsContainer())
            .OnCreateCell(this, &SOdysseyAnimationLayerImageTimeline::OnCreateCell)
            .OnCreateCellWidget(this, &SOdysseyAnimationLayerImageTimeline::OnGenerateCellWidget)
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationTimelineFrameSelector, mExtension)
            .Visibility(this, &SOdysseyAnimationLayerImageTimeline::GetFrameSelectorVisibility)
        ]
    ];
}

EVisibility
SOdysseyAnimationLayerImageTimeline::GetFrameSelectorVisibility() const
{
    bool isCurrentLayer = mLayer->GetLayerStack()->CurrentLayer == mLayer;
	return isCurrentLayer ? EVisibility::Visible : EVisibility::Hidden;
}

FReply
SOdysseyAnimationLayerImageTimeline::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	if (iEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        int frame = mExtension->Timeline()->GetFrameIndexAtMousePosition(iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition()).X);
        if (frame == INDEX_NONE)
            return FReply::Unhandled();

		FMenuBuilder menuBuilder(true, mCommandList);
		BuildContextMenu(menuBuilder, frame);

		TSharedRef<SWidget> menuContents = menuBuilder.MakeWidget();
		FWidgetPath widgetPath = iEvent.GetEventPath() != nullptr ? *iEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), widgetPath, menuContents, iEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
    	return FReply::Handled();
	}
	return FReply::Unhandled();
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
    FScopedTransaction ScopedTransaction(LOCTEXT("Timeline", "Cut Frames"));
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
    FScopedTransaction ScopedTransaction(LOCTEXT("Timeline", "Paste Frames"));
#endif

    clipboardData->Paste(mLayer, mExtension->Animation()->CurrentFrame);
}

void
SOdysseyAnimationLayerImageTimeline::DeleteSelectedFrames()
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Remove Frames"));
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
SOdysseyAnimationLayerImageTimeline::BuildContextMenu(FMenuBuilder& iMenuBuilder, int iFrame)
{
    const FText commonSectionTitle = LOCTEXT("OdysseyAnimationTimelineCommonSection", "Common");
    iMenuBuilder.PushCommandList(mCommandList);
    iMenuBuilder.BeginSection("Selection", LOCTEXT("LayerStackCommonSection", "Selection"));
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().SelectAll);
    iMenuBuilder.EndSection();

    iMenuBuilder.BeginSection("Common", LOCTEXT("LayerStackCommonSection", "Common"));
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
        iMenuBuilder.AddSeparator("");
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Cut);
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Paste);
        iMenuBuilder.AddSeparator("");
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);

    iMenuBuilder.EndSection();
    iMenuBuilder.PopCommandList();
}

void
SOdysseyAnimationLayerImageTimeline::MapActions(TSharedPtr<FUICommandList> iCommandList)
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
}

#undef LOCTEXT_NAMESPACE
