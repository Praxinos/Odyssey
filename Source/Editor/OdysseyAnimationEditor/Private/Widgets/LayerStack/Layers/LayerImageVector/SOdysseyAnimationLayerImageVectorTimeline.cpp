// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageVectorTimeline"

SOdysseyAnimationLayerImageVectorTimeline::~SOdysseyAnimationLayerImageVectorTimeline()
{
}

SOdysseyAnimationLayerImageVectorTimeline::SOdysseyAnimationLayerImageVectorTimeline()
    : mCommandList(MakeShared<FUICommandList>())
{
    MapActions(mCommandList);
}

void
SOdysseyAnimationLayerImageVectorTimeline::Construct(
    const FArguments& InArgs,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
)
{
    ensure(iAnimationLayerImageVector);

    mExtension = iExtension;
    mAnimationLayerImageVector = iAnimationLayerImageVector;
    
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationCells, mExtension, mAnimationLayerImageVector, mAnimationLayerImageVector->GetCellsContainer())
            .OnCreateCell(this, &SOdysseyAnimationLayerImageVectorTimeline::OnCreateCell)
            .OnCreateCellWidget(this, &SOdysseyAnimationLayerImageVectorTimeline::OnCreateCellWidget)
            .OnBuildContextMenu(this, &SOdysseyAnimationLayerImageVectorTimeline::OnBuildCellsContextMenu)
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationTimelineFrameSelector, mExtension)
            .Visibility(this, &SOdysseyAnimationLayerImageVectorTimeline::GetFrameSelectorVisibility)
            .OnBuildContextMenu(this, &SOdysseyAnimationLayerImageVectorTimeline::OnBuildFrameSelectorContextMenu)
        ]
    ];
}

TSharedRef<FOdysseyAnimationCell>
SOdysseyAnimationLayerImageVectorTimeline::OnCreateCell()
{
    UOdysseyAnimation* animation = mAnimationLayerImageVector->GetAnimation();
    return FOdysseyAnimationCellImageVector::Create(mAnimationLayerImageVector, 1, animation->Width(), animation->Height());
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorTimeline::OnCreateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell)
{
    return SNew(SOdysseyAnimationLayerImageVectorCell);
}

EVisibility
SOdysseyAnimationLayerImageVectorTimeline::GetFrameSelectorVisibility() const
{
    bool isCurrentLayer = mAnimationLayerImageVector->GetLayerStack()->CurrentLayer == mAnimationLayerImageVector;
	return isCurrentLayer ? EVisibility::Visible : EVisibility::Hidden;
}

void
SOdysseyAnimationLayerImageVectorTimeline::OnBuildCellsContextMenu(FMenuBuilder& iMenuBuilder, int iFrame)
{
    BuildContextMenu(iMenuBuilder, iFrame);
}

void
SOdysseyAnimationLayerImageVectorTimeline::OnBuildFrameSelectorContextMenu(FMenuBuilder& iMenuBuilder, int iFrame)
{
    BuildContextMenu(iMenuBuilder, iFrame);
}

bool
SOdysseyAnimationLayerImageVectorTimeline::SupportsKeyboardFocus() const
{
    return true;
}

FReply
SOdysseyAnimationLayerImageVectorTimeline::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
	if (mCommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SCompoundWidget::OnKeyDown(iGeometry, iKeyEvent);
}

FReply
SOdysseyAnimationLayerImageVectorTimeline::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
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
SOdysseyAnimationLayerImageVectorTimeline::SelectAllFrames()
{
    FInt32Range frameRange = mAnimationLayerImageVector->GetFrameRange();
    mExtension->Timeline()->SetSelectedFrames(frameRange);
}

void
SOdysseyAnimationLayerImageVectorTimeline::DeleteSelectedFrames()
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Remove Frames"));
#endif

    FOdysseyAnimationCellsMutator mutator(mAnimationLayerImageVector, mAnimationLayerImageVector->GetCellsContainer());

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
SOdysseyAnimationLayerImageVectorTimeline::BuildContextMenu(FMenuBuilder& iMenuBuilder, int iFrame)
{
    const FText commonSectionTitle = LOCTEXT("OdysseyAnimationTimelineCommonSection", "Common");
    iMenuBuilder.BeginSection("Common", commonSectionTitle);
        iMenuBuilder.PushCommandList(mCommandList);
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().SelectAll);
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
        iMenuBuilder.PopCommandList();
    iMenuBuilder.EndSection();
}

void
SOdysseyAnimationLayerImageVectorTimeline::MapActions(TSharedPtr<FUICommandList> iCommandList)
{
	iCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageVectorTimeline::SelectAllFrames)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageVectorTimeline::DeleteSelectedFrames)
    );
}

#undef LOCTEXT_NAMESPACE
