// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterCell.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageRasterCell"

void
SOdysseyAnimationLayerImageRasterCell::Construct(
    const FArguments& iArgs,
	FOdysseyAnimationEditorExtension* iExtension,
	UOdysseyAnimationLayerImageRaster* iLayer
)
{
	mExtension = iExtension;
	mLayer = iLayer;
	mOnBuildContextMenu = iArgs._OnBuildContextMenu;
	mOnMapActions = iArgs._OnMapActions;

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameBackground"))
		.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f))
	];
}

bool
SOdysseyAnimationLayerImageRasterCell::SupportsKeyboardFocus() const
{
	return true;
}

FReply
SOdysseyAnimationLayerImageRasterCell::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
	TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
	mOnMapActions.ExecuteIfBound(commandList);
	if (commandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SCompoundWidget::OnKeyDown(iGeometry, iKeyEvent);
}

FReply
SOdysseyAnimationLayerImageRasterCell::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	if (iEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
		TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
		mOnMapActions.ExecuteIfBound(commandList);

		FMenuBuilder menuBuilder(true, commandList);
		mOnBuildContextMenu.ExecuteIfBound(menuBuilder);

		TSharedRef<SWidget> menuContents = menuBuilder.MakeWidget();
		FWidgetPath widgetPath = iEvent.GetEventPath() != nullptr ? *iEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), widgetPath, menuContents, iEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
    	return FReply::Handled();
	}
	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE