// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageRasterTimeline"

SOdysseyAnimationLayerImageRasterTimeline::~SOdysseyAnimationLayerImageRasterTimeline()
{
}

SOdysseyAnimationLayerImageRasterTimeline::SOdysseyAnimationLayerImageRasterTimeline()
    : mCommandList(MakeShared<FUICommandList>())
{
    MapActions(mCommandList);
}

void
SOdysseyAnimationLayerImageRasterTimeline::Construct(
    const FArguments& InArgs,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
)
{
    ensure(iAnimationLayerImageRaster);

    mExtension = iExtension;
    mAnimationLayerImageRaster = iAnimationLayerImageRaster;
    
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationCells, mExtension, mAnimationLayerImageRaster, mAnimationLayerImageRaster->GetCellsContainer())
            .OnCreateCell(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCreateCell)
            .OnCreateCellWidget(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCreateCellWidget)
            .OnBuildContextMenu(this, &SOdysseyAnimationLayerImageRasterTimeline::OnBuildCellsContextMenu)
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationTimelineFrameSelector, mExtension)
            .Visibility(this, &SOdysseyAnimationLayerImageRasterTimeline::GetFrameSelectorVisibility)
            .OnBuildContextMenu(this, &SOdysseyAnimationLayerImageRasterTimeline::OnBuildFrameSelectorContextMenu)
        ]
    ];
}

TSharedRef<FOdysseyAnimationCell>
SOdysseyAnimationLayerImageRasterTimeline::OnCreateCell()
{
    UOdysseyAnimation* animation = mAnimationLayerImageRaster->GetAnimation();
    return FOdysseyAnimationCellImageRaster::Create(mAnimationLayerImageRaster, 1, animation->Width(), animation->Height(), animation->Format());
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterTimeline::OnCreateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell)
{
    return SNew(SOdysseyAnimationLayerImageRasterCell);
}

EVisibility
SOdysseyAnimationLayerImageRasterTimeline::GetFrameSelectorVisibility() const
{
    bool isCurrentLayer = mAnimationLayerImageRaster->GetLayerStack()->CurrentLayer == mAnimationLayerImageRaster;
	return isCurrentLayer ? EVisibility::Visible : EVisibility::Hidden;
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnBuildCellsContextMenu(FMenuBuilder& iMenuBuilder, int iFrame)
{
    BuildContextMenu(iMenuBuilder, iFrame);
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnBuildFrameSelectorContextMenu(FMenuBuilder& iMenuBuilder, int iFrame)
{
    BuildContextMenu(iMenuBuilder, iFrame);
}

bool
SOdysseyAnimationLayerImageRasterTimeline::SupportsKeyboardFocus() const
{
    return true;
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
	if (mCommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SCompoundWidget::OnKeyDown(iGeometry, iKeyEvent);
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
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
SOdysseyAnimationLayerImageRasterTimeline::SelectAllFrames()
{
    FInt32Range frameRange = mAnimationLayerImageRaster->GetFrameRange();
    mExtension->Timeline()->SetSelectedFrames(frameRange);
}

void
SOdysseyAnimationLayerImageRasterTimeline::DeleteSelectedFrames()
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Remove Frames"));
#endif

    FOdysseyAnimationCellsMutator mutator(mAnimationLayerImageRaster, mAnimationLayerImageRaster->GetCellsContainer());

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
SOdysseyAnimationLayerImageRasterTimeline::CopyFrames()
{
    FOdysseyAnimationCellClipboard::Get()->Copy(mAnimationLayerImageRaster->GetCellsContainer(), mExtension->Timeline()->GetSelectedFrames());
}

void
SOdysseyAnimationLayerImageRasterTimeline::CutFrames()
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Timeline", "Cut Frames"));
#endif
    FOdysseyAnimationCellClipboard::Get()->Copy(mAnimationLayerImageRaster->GetCellsContainer(), mExtension->Timeline()->GetSelectedFrames());
    DeleteSelectedFrames();
}

void
SOdysseyAnimationLayerImageRasterTimeline::PasteFrames()
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Timeline", "Paste Frames"));
#endif
    FOdysseyAnimationCellClipboard::Get()->Paste(mAnimationLayerImageRaster, mAnimationLayerImageRaster->GetCellsContainer(), mExtension->Animation()->CurrentFrame);
}

void
SOdysseyAnimationLayerImageRasterTimeline::BuildContextMenu(FMenuBuilder& iMenuBuilder, int iFrame)
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
SOdysseyAnimationLayerImageRasterTimeline::MapActions(TSharedPtr<FUICommandList> iCommandList)
{
	iCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageRasterTimeline::SelectAllFrames)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageRasterTimeline::DeleteSelectedFrames)
    );

	iCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageRasterTimeline::CopyFrames)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageRasterTimeline::CutFrames)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageRasterTimeline::PasteFrames)
    );
}

#undef LOCTEXT_NAMESPACE
