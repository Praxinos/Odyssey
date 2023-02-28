// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "DragDropOperations/OdysseyLayerStackDragDropOperation.h"

#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyStyleSet.h"

FOdysseyLayerStackDragDropOperation::FOdysseyLayerStackDragDropOperation(UOdysseyLayerStack* iLayerStack, TArray<UOdysseyLayer*> iLayers)
	: mLayerStack(iLayerStack)
	, mLayers(iLayers)
{
}

TSharedPtr<SWidget>
FOdysseyLayerStackDragDropOperation::GetDefaultDecorator() const
{
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("Graph.ConnectorFeedback.Border")) //weird, but everyone does this 
		.Content()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.f, 0.f, 2.f, 0.f)
			[
				SNew(SImage)
				.Image(this, &FOdysseyLayerStackDragDropOperation::GetIcon)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(this, &FOdysseyLayerStackDragDropOperation::GetText)
			]
		];
}

FText
FOdysseyLayerStackDragDropOperation::GetText() const
{
	if (mLayers.Num() == 1)
		return mLayers[0]->Name;
	
	return FText::Format(NSLOCTEXT("OdysseyLayerStackDragDropOperation", "LayerCount", "{0} Layers"), FText::AsNumber(mLayers.Num()));
}

const FSlateBrush*
FOdysseyLayerStackDragDropOperation::GetIcon() const
{
	if (mLayers.Num() == 1)
		return &mLayers[0]->Icon;

	return FOdysseyStyle::GetBrush("PainterEditor.Layers16");
}

TArray< UOdysseyLayer* >
FOdysseyLayerStackDragDropOperation::GetTopmostLayers() const
{
	TArray< UOdysseyLayer* > topmostLayers;
	for( UOdysseyLayer* layer : mLayers)
	{
		bool hasParentInDraggedLayers = false;
		TArray<UOdysseyLayer*> parents = layer->GetParents();
		for ( UOdysseyLayer* parent : parents )
		{
			if ( mLayers.Contains(parent) )
			{
				hasParentInDraggedLayers = true;
				break;
			}
		}

		if (hasParentInDraggedLayers)
			continue;

		topmostLayers.Add(layer);
	}
	return topmostLayers;
}

TArray< UOdysseyLayer* >
FOdysseyLayerStackDragDropOperation::GetLayers() const
{
	return mLayers;
}

UOdysseyLayerStack*
FOdysseyLayerStackDragDropOperation::GetLayerStack() const
{
	return mLayerStack;
}