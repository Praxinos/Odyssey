// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/SOdysseyLayerStackPropertyViewTreeNode.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBox.h"
#include "OdysseyImageLayer.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"


#define LOCTEXT_NAMESPACE "OdysseyLayerStackPropertyViewTreeNode"

//CONSTRUCTION/DESTRUCTION--------------------------------------

SOdysseyLayerStackPropertyViewTreeNode::~SOdysseyLayerStackPropertyViewTreeNode()
{
}



void SOdysseyLayerStackPropertyViewTreeNode::Construct( const FArguments& InArgs, TSharedRef<OdysseyBaseLayerNode> Node )
{
    FOdysseyLayerStack* OdysseyLayerStackPtr = Node->GetLayerStack().GetLayerStackData().Get();
    IOdysseyLayer::eType LayerType = Node->GetLayerDataPtr()->GetType();

    TSharedRef<SWidget> FinalWidget = SNullWidget::NullWidget;

    switch( LayerType )
    {
        case IOdysseyLayer::eType::kImage :
            FOdysseyImageLayer* ImageLayer = static_cast<FOdysseyImageLayer*> (Node->GetLayerDataPtr());
            TSharedRef<OdysseyTrackLayerNode> trackNode = StaticCastSharedRef<OdysseyTrackLayerNode>(Node);
            FinalWidget = ConstructPropertyViewForImageLayer( ImageLayer, OdysseyLayerStackPtr, trackNode );
        break;
    }

    ChildSlot
    [
        FinalWidget
    ];
}


//PRIVATE API


TSharedRef<SWidget> SOdysseyLayerStackPropertyViewTreeNode::ConstructPropertyViewForImageLayer( FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyTrackLayerNode> trackNode )
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
                .Value(this, &SOdysseyLayerStackPropertyViewTreeNode::GetLayerOpacityValue, ImageLayer)
                .MinValue(0)
                .MaxValue(100)
                .Delta(1)
                .OnValueChanged(this, &SOdysseyLayerStackPropertyViewTreeNode::HandleLayerOpacityValueChanged, ImageLayer, LayerStack, trackNode )
                .OnValueCommitted(this, &SOdysseyLayerStackPropertyViewTreeNode::SetLayerOpacityValue, ImageLayer, LayerStack, trackNode )
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
                .OnGenerateWidget(this, &SOdysseyLayerStackPropertyViewTreeNode::GenerateBlendingComboBoxItem)
                .OnSelectionChanged(this, &SOdysseyLayerStackPropertyViewTreeNode::HandleOnBlendingModeChanged, ImageLayer, LayerStack, trackNode )
                .Content()
                [
                    //The text in the main button
                    CreateBlendingModeTextWidget( ImageLayer )
                ]
             ]
        ];


    return FinalWidget;
}



int SOdysseyLayerStackPropertyViewTreeNode::GetLayerOpacityValue( FOdysseyImageLayer* ImageLayer ) const
{
    return ImageLayer->GetOpacity() * 100;
}


void SOdysseyLayerStackPropertyViewTreeNode::HandleLayerOpacityValueChanged( int iOpacity, FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyTrackLayerNode> TrackNode  )
{
    ImageLayer->SetOpacity( iOpacity / 100.f );
    TrackNode->RefreshOpacityText();
}

void SOdysseyLayerStackPropertyViewTreeNode::SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyTrackLayerNode> TrackNode  )
{
    ImageLayer->SetOpacity( iOpacity / 100.f );
    TrackNode->RefreshOpacityText();
    LayerStack->ComputeResultBlock();
}


//PRIVATE

void SOdysseyLayerStackPropertyViewTreeNode::HandleOnBlendingModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo, FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyTrackLayerNode> TrackNode )
{
    ImageLayer->SetBlendingMode( *(NewSelection.Get() ) );
    TrackNode->RefreshBlendingModeText();
    LayerStack->ComputeResultBlock();

    BlendingModeComboBox->SetContent(
        SNew(   SComboBox<TSharedPtr<FText>> )
                .OptionsSource(&BlendingModes)
                .OnGenerateWidget(this, &SOdysseyLayerStackPropertyViewTreeNode::GenerateBlendingComboBoxItem)
                .OnSelectionChanged(this, &SOdysseyLayerStackPropertyViewTreeNode::HandleOnBlendingModeChanged, ImageLayer, LayerStack, TrackNode )
                .Content()
                [
                    //The text in the main button
                    CreateBlendingModeTextWidget( ImageLayer )
                ]
    );
}



TSharedRef<SWidget> SOdysseyLayerStackPropertyViewTreeNode::GenerateBlendingComboBoxItem(TSharedPtr<FText> InItem)
{
      return SNew(STextBlock)
           .Text(*(InItem.Get()));
}


TSharedRef<SWidget> SOdysseyLayerStackPropertyViewTreeNode::CreateBlendingModeTextWidget( FOdysseyImageLayer* imageLayer)
{
      return SNew(STextBlock)
           .Text( imageLayer->GetBlendingModeAsText() );
}




#undef LOCTEXT_NAMESPACE
