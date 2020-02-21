// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/LayersGUI/SOdysseyFolderLayerNodePropertyView.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBox.h"
#include "OdysseyFolderLayer.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"


#define LOCTEXT_NAMESPACE "OdysseyFolderLayerNodePropertyView"

//CONSTRUCTION/DESTRUCTION--------------------------------------

SOdysseyFolderLayerNodePropertyView::~SOdysseyFolderLayerNodePropertyView()
{
}



void SOdysseyFolderLayerNodePropertyView::Construct( const FArguments& InArgs, TSharedRef<OdysseyBaseLayerNode> Node )
{
    FOdysseyLayerStack* OdysseyLayerStackPtr = Node->GetLayerStack().GetLayerStackData().Get();
    IOdysseyLayer::eType LayerType = Node->GetLayerDataPtr()->GetType();

    TSharedRef<SWidget> FinalWidget = SNullWidget::NullWidget;

    FOdysseyFolderLayer* FolderLayer = static_cast<FOdysseyFolderLayer*> (Node->GetLayerDataPtr());
    TSharedRef<OdysseyFolderLayerNode> trackNode = StaticCastSharedRef<OdysseyFolderLayerNode>(Node);
    FinalWidget = ConstructPropertyViewForFolderLayer( FolderLayer, OdysseyLayerStackPtr, trackNode );


    ChildSlot
    [
        FinalWidget
    ];
}


//PRIVATE API


TSharedRef<SWidget> SOdysseyFolderLayerNodePropertyView::ConstructPropertyViewForFolderLayer( FOdysseyFolderLayer* FolderLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyFolderLayerNode> trackNode )
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
                .Value(this, &SOdysseyFolderLayerNodePropertyView::GetLayerOpacityValue, FolderLayer)
                .MinValue(0)
                .MaxValue(100)
                .Delta(1)
                .OnValueChanged(this, &SOdysseyFolderLayerNodePropertyView::HandleLayerOpacityValueChanged, FolderLayer, LayerStack, trackNode )
                .OnValueCommitted(this, &SOdysseyFolderLayerNodePropertyView::SetLayerOpacityValue, FolderLayer, LayerStack, trackNode )
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
                .OnGenerateWidget(this, &SOdysseyFolderLayerNodePropertyView::GenerateBlendingComboBoxItem)
                .OnSelectionChanged(this, &SOdysseyFolderLayerNodePropertyView::HandleOnBlendingModeChanged, FolderLayer, LayerStack, trackNode )
                .Content()
                [
                    //The text in the main button
                    CreateBlendingModeTextWidget( FolderLayer )
                ]
             ]
        ];


    return FinalWidget;
}



int SOdysseyFolderLayerNodePropertyView::GetLayerOpacityValue( FOdysseyFolderLayer* FolderLayer ) const
{
    return FolderLayer->GetOpacity() * 100;
}


void SOdysseyFolderLayerNodePropertyView::HandleLayerOpacityValueChanged( int iOpacity, FOdysseyFolderLayer* FolderLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyFolderLayerNode> TrackNode  )
{
    FolderLayer->SetOpacity( iOpacity / 100.f );
    TrackNode->RefreshOpacityText();
}

void SOdysseyFolderLayerNodePropertyView::SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, FOdysseyFolderLayer* FolderLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyFolderLayerNode> TrackNode  )
{
    FolderLayer->SetOpacity( iOpacity / 100.f );
    TrackNode->RefreshOpacityText();
    LayerStack->ComputeResultBlock();
}


//PRIVATE

void SOdysseyFolderLayerNodePropertyView::HandleOnBlendingModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo, FOdysseyFolderLayer* FolderLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyFolderLayerNode> TrackNode )
{
    FolderLayer->SetBlendingMode( *(NewSelection.Get() ) );
    TrackNode->RefreshBlendingModeText();
    LayerStack->ComputeResultBlock();

    BlendingModeComboBox->SetContent(
        SNew(   SComboBox<TSharedPtr<FText>> )
                .OptionsSource(&BlendingModes)
                .OnGenerateWidget(this, &SOdysseyFolderLayerNodePropertyView::GenerateBlendingComboBoxItem)
                .OnSelectionChanged(this, &SOdysseyFolderLayerNodePropertyView::HandleOnBlendingModeChanged, FolderLayer, LayerStack, TrackNode )
                .Content()
                [
                    //The text in the main button
                    CreateBlendingModeTextWidget( FolderLayer )
                ]
    );
}



TSharedRef<SWidget> SOdysseyFolderLayerNodePropertyView::GenerateBlendingComboBoxItem(TSharedPtr<FText> InItem)
{
      return SNew(STextBlock)
           .Text(*(InItem.Get()));
}


TSharedRef<SWidget> SOdysseyFolderLayerNodePropertyView::CreateBlendingModeTextWidget( FOdysseyFolderLayer* FolderLayer)
{
      return SNew(STextBlock)
           .Text( FolderLayer->GetBlendingModeAsText() );
}




#undef LOCTEXT_NAMESPACE
