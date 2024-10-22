// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyAnimationComponentTrack.h"

#include "Widgets/LayerStack/SOdysseyAnimationTimelineToolSelector.h"
#include "Widgets/SOdysseyLayerStackAddLayerButton.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationComponentTrack::Construct(const FArguments& iArgs, UOdysseyAnimationComponent* iComponent)
{
	ensure(iComponent);
	mComponent = iComponent;

    RebuildWidgets();
}

void
SOdysseyAnimationComponentTrack::RebuildWidgets()
{
	this->ChildSlot.DetachWidget();

	UOdysseyAnimation* animation = mComponent->GetActiveAnimation();
	if (!animation)
		return;

	TSharedPtr<SWidget> widget = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SOdysseyLayerStackAddLayerButton)
				.LayerStack(animation->GetLayerStack())
				.OnAdded( this, &SOdysseyAnimationComponentTrack::OnLayerAdded)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SOdysseyAnimationTimelineToolSelector)
			]
		]
		+ SVerticalBox::Slot()
		[
			SNew(SOdysseyAnimationLayerStackTreeView)
			.LayerStack(animation->GetLayerStack())
			.HeaderHeight(20)
			.ExternalScrollbar(SNew(SScrollBar))
		];

	this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

void
SOdysseyAnimationComponentTrack::OnLayerAdded(UOdysseyLayer* iLayer)
{
    if (iLayer->GetClass() == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
		UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(iLayer);
		layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
    }
    else if (iLayer->GetClass() == UOdysseyAnimationLayerImageVector::StaticClass())
    {
		UOdysseyAnimationLayerImageVector* layer = Cast<UOdysseyAnimationLayerImageVector>(iLayer);
        layer->AddCell(UOdysseyAnimationCellImageVector::StaticClass());
    }
}

#undef LOCTEXT_NAMESPACE