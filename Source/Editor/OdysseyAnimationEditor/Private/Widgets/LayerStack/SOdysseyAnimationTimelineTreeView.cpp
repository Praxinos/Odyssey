// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineTreeView.h"

#include "Shortcuts/Timeline/OdysseyAnimationTimelineShortcuts.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationTimelineTreeView::Construct(const FArguments& InArgs)
{
	mTimelineCellSelection = InArgs._TimelineCellSelection;
	mTimelinePosition = InArgs._TimelinePosition;
	mLayerStack = InArgs._LayerStack;
	mPlayer = InArgs._Player;
	mOnActivateOutOfPegs = InArgs._OnActivateOutOfPegs;
	mOnInactivateOutOfPegs = InArgs._OnInactivateOutOfPegs;
	mOnIsOutOfPegsChecked = InArgs._OnIsOutOfPegsChecked;

    mTimelineShortcuts = MakeShared<FOdysseyAnimationTimelineShortcuts>(mLayerStack, mTimelineCellSelection);

	TArray<SHeaderRow::FColumn::FArguments> columns = {
		SHeaderRow::Column("Timeline")
		.DefaultLabel(FText())
		.VAlignCell(VAlign_Fill)
		.HAlignCell(HAlign_Fill)
		[
			SNew(SBox)
			.HeightOverride(InArgs._HeaderHeight)
			[
				SNew(SOdysseyAnimationTimelineHeader)
				.Animation(mLayerStack->GetAnimation())
				.Player(InArgs._Player)
				.TimelinePosition(InArgs._TimelinePosition)
			]
		]
	};

	ChildSlot
	[
		SAssignNew(mTimelineControl, SOdysseyAnimationTimelineControl)
		.Animation(mLayerStack->GetAnimation())
		.CurrentFrame(this, &SOdysseyAnimationTimelineTreeView::GetCurrentFrame)
		.TimelinePosition(InArgs._TimelinePosition)
		[
			SAssignNew(mTreeView, SOdysseyAnimationLayerStackTreeView)
			.LayerStack(mLayerStack)
			.OnGenerateRow( this, &SOdysseyAnimationTimelineTreeView::OnGenerateRow )
			.Columns(columns)
			//.SelectionMode( ESelectionMode::None )
		]
	];
}

TSharedPtr<SOdysseyAnimationLayerStackTreeView>
SOdysseyAnimationTimelineTreeView::GetTreeView() const
{
	return mTreeView;
}

FReply
SOdysseyAnimationTimelineTreeView::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
	if (mTimelineShortcuts->GetCommandList()->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SCompoundWidget::OnKeyDown(iGeometry, iKeyEvent);
}

TSharedRef<ITableRow>
SOdysseyAnimationTimelineTreeView::OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable)
{
	check(iLayer);

    UClass* layerClass = iLayer->GetClass();
    if (layerClass == UOdysseyAnimationLayerFolder::StaticClass())
    {
		return SNew(SOdysseyAnimationLayerFolderTimeline, GetTreeView().ToSharedRef(), Cast<UOdysseyAnimationLayerFolder>(iLayer));
	}
	if (layerClass == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
		return SNew(SOdysseyAnimationLayerImageRasterTimeline, GetTreeView().ToSharedRef(), Cast<UOdysseyAnimationLayerImageRaster>(iLayer))
			.TimelinePosition(mTimelinePosition)
			.TimelineCellSelection(mTimelineCellSelection)
			.OnActivateOutOfPegs(mOnActivateOutOfPegs)
			.OnInactivateOutOfPegs(mOnInactivateOutOfPegs)
			.OnIsOutOfPegsChecked(mOnIsOutOfPegsChecked);
	}
	if (layerClass == UOdysseyAnimationLayerImageVector::StaticClass())
    {
		return SNew(SOdysseyAnimationLayerImageVectorTimeline, GetTreeView().ToSharedRef(), Cast<UOdysseyAnimationLayerImageVector>(iLayer))
			.TimelinePosition(mTimelinePosition)
			.TimelineCellSelection(mTimelineCellSelection)
			.OnActivateOutOfPegs(mOnActivateOutOfPegs)
			.OnInactivateOutOfPegs(mOnInactivateOutOfPegs)
			.OnIsOutOfPegsChecked(mOnIsOutOfPegsChecked);
	}

    return SNew(STableRow<UOdysseyLayer*>, iOwnerTable);;
}

int
SOdysseyAnimationTimelineTreeView::GetCurrentFrame() const
{
	if (!mPlayer)
		return 0;

	UOdysseyAnimation* animation = mPlayer->Animation;
	if (!animation)
		return 0;

	return animation->GetFrameIndexAtTime(mPlayer->GetCurrentTime());
}

#undef LOCTEXT_NAMESPACE
