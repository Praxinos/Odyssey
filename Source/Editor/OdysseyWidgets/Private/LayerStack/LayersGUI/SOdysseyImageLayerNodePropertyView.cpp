// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/LayersGUI/SOdysseyImageLayerNodePropertyView.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBox.h"
#include "OdysseyImageLayer.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"


#define LOCTEXT_NAMESPACE "OdysseyImageLayerNodePropertyView"

//CONSTRUCTION/DESTRUCTION--------------------------------------

SOdysseyImageLayerNodePropertyView::~SOdysseyImageLayerNodePropertyView()
{
}



void SOdysseyImageLayerNodePropertyView::Construct( const FArguments& InArgs, TSharedRef<OdysseyBaseLayerNode> Node )
{
    FOdysseyLayerStack* OdysseyLayerStackPtr = Node->GetLayerStack().GetLayerStackData().Get();
    IOdysseyLayer::eType LayerType = Node->GetLayerDataPtr()->GetType();

    TSharedRef<SWidget> FinalWidget = SNullWidget::NullWidget;

    FOdysseyImageLayer* ImageLayer = static_cast<FOdysseyImageLayer*> (Node->GetLayerDataPtr());
    TSharedRef<OdysseyImageLayerNode> trackNode = StaticCastSharedRef<OdysseyImageLayerNode>(Node);
    FinalWidget = ConstructPropertyViewForImageLayer( ImageLayer, OdysseyLayerStackPtr, trackNode );

    ChildSlot
    [
        FinalWidget
    ];
}


//PRIVATE API


TSharedRef<SWidget> SOdysseyImageLayerNodePropertyView::ConstructPropertyViewForImageLayer( FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyImageLayerNode> trackNode )
{
    BlendingModes = LayerStack->GetBlendingModesAsText();

    TSharedRef<SWidget>    FinalWidget =
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
        [
            SNew(SHorizontalBox )
            + SHorizontalBox::Slot()
            .FillWidth(0.5f)
            [
                SNew( STextBlock )
                .Text(LOCTEXT("Opacity", "Opacity"))
            ]

            +SHorizontalBox::Slot()
            .Padding( FMargin( 20.f, 0.f, 0.f, 0.f) )
            .FillWidth(0.5f)
            .VAlign( VAlign_Center )
            [
                SNew(SSpinBox<int>)
                //.Style( FEditorStyle::Get(), "NoBorder" )
                .Value(this, &SOdysseyImageLayerNodePropertyView::GetLayerOpacityValue, ImageLayer)
                .MinValue(0)
                .MaxValue(100)
                .Delta(1)
                .OnValueChanged(this, &SOdysseyImageLayerNodePropertyView::HandleLayerOpacityValueChanged, ImageLayer, LayerStack, trackNode )
                .OnValueCommitted(this, &SOdysseyImageLayerNodePropertyView::SetLayerOpacityValue, ImageLayer, LayerStack, trackNode )
             ]
         ]
        + SVerticalBox::Slot()
        .Padding( FMargin( 0.f, 3.f, 0.f, 0.f) )
        [
            SNew(SHorizontalBox )
            + SHorizontalBox::Slot()
            .FillWidth(0.5f)
            [
                SNew( STextBlock )
                .Text(LOCTEXT("Blending Mode", "Blending Mode"))
            ]

             + SHorizontalBox::Slot()
             .Padding( FMargin( 20.f, 0.f, 0.f, 0.f) )
             .FillWidth(0.5f)
             .VAlign( VAlign_Center )
             [
                SAssignNew( BlendingModeComboBox, SComboBox<TSharedPtr<FText>>)
                .OptionsSource(&BlendingModes)
                .OnGenerateWidget(this, &SOdysseyImageLayerNodePropertyView::GenerateBlendingComboBoxItem)
                .OnSelectionChanged(this, &SOdysseyImageLayerNodePropertyView::HandleOnBlendingModeChanged, ImageLayer, LayerStack, trackNode )
                .Content()
                [
                    //The text in the main button
                    CreateBlendingModeTextWidget( ImageLayer )
                ]
             ]
        ];


    return FinalWidget;
}



int SOdysseyImageLayerNodePropertyView::GetLayerOpacityValue( FOdysseyImageLayer* ImageLayer ) const
{
    return ImageLayer->GetOpacity() * 100;
}


void SOdysseyImageLayerNodePropertyView::HandleLayerOpacityValueChanged( int iOpacity, FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyImageLayerNode> TrackNode  )
{
    ImageLayer->SetOpacity( iOpacity / 100.f );
    TrackNode->RefreshOpacityText();
}

void SOdysseyImageLayerNodePropertyView::SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyImageLayerNode> TrackNode  )
{
    ImageLayer->SetOpacity( iOpacity / 100.f );
    TrackNode->RefreshOpacityText();
    LayerStack->ComputeResultBlock();
}


//PRIVATE

void SOdysseyImageLayerNodePropertyView::HandleOnBlendingModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo, FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyImageLayerNode> TrackNode )
{
    ImageLayer->SetBlendingMode( *(NewSelection.Get() ) );
    TrackNode->RefreshBlendingModeText();
    LayerStack->ComputeResultBlock();

    BlendingModeComboBox->SetContent(
        SNew(   SComboBox<TSharedPtr<FText>> )
                .OptionsSource(&BlendingModes)
                .OnGenerateWidget(this, &SOdysseyImageLayerNodePropertyView::GenerateBlendingComboBoxItem)
                .OnSelectionChanged(this, &SOdysseyImageLayerNodePropertyView::HandleOnBlendingModeChanged, ImageLayer, LayerStack, TrackNode )
                .Content()
                [
                    //The text in the main button
                    CreateBlendingModeTextWidget( ImageLayer )
                ]
    );
}



TSharedRef<SWidget> SOdysseyImageLayerNodePropertyView::GenerateBlendingComboBoxItem(TSharedPtr<FText> InItem)
{
      return SNew(STextBlock)
           .Text(*(InItem.Get()));
}


TSharedRef<SWidget> SOdysseyImageLayerNodePropertyView::CreateBlendingModeTextWidget( FOdysseyImageLayer* imageLayer)
{
      return SNew(STextBlock)
           .Text( imageLayer->GetBlendingModeAsText() );
}




#undef LOCTEXT_NAMESPACE
