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


#define LOCTEXT_NAMESPACE "SOdysseyFolderLayerNodePropertyView"

//CONSTRUCTION/DESTRUCTION--------------------------------------

SOdysseyFolderLayerNodePropertyView::~SOdysseyFolderLayerNodePropertyView()
{
}



void SOdysseyFolderLayerNodePropertyView::Construct( const FArguments& InArgs, TSharedRef<IOdysseyBaseLayerNode> iNode )
{
    FOdysseyLayerStack* odysseyLayerStackPtr = iNode->GetLayerStack().GetLayerStackData().Get();
    IOdysseyLayer::eType layerType = iNode->GetLayerDataPtr()->GetType();

    TSharedRef<SWidget> finalWidget = SNullWidget::NullWidget;

    FOdysseyFolderLayer* folderLayer = static_cast<FOdysseyFolderLayer*> (iNode->GetLayerDataPtr());
    TSharedRef<FOdysseyFolderLayerNode> folderNode = StaticCastSharedRef<FOdysseyFolderLayerNode>(iNode);
    finalWidget = ConstructPropertyViewForFolderLayer( folderLayer, odysseyLayerStackPtr, folderNode );


    ChildSlot
    [
        finalWidget
    ];
}


//PRIVATE API


TSharedRef<SWidget> SOdysseyFolderLayerNodePropertyView::ConstructPropertyViewForFolderLayer( FOdysseyFolderLayer* iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode )
{
    mBlendingModes = iLayerStack->GetBlendingModesAsText();

    TSharedRef<SWidget>    finalWidget =
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
                .Value(this, &SOdysseyFolderLayerNodePropertyView::GetLayerOpacityValue, iFolderLayer)
                .MinValue(0)
                .MaxValue(100)
                .Delta(1)
                .OnValueChanged(this, &SOdysseyFolderLayerNodePropertyView::HandleLayerOpacityValueChanged, iFolderLayer, iLayerStack, iFolderNode )
                .OnValueCommitted(this, &SOdysseyFolderLayerNodePropertyView::SetLayerOpacityValue, iFolderLayer, iLayerStack, iFolderNode )
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
                SAssignNew( mBlendingModeComboBox, SComboBox<TSharedPtr<FText>>)
                .OptionsSource(&mBlendingModes)
                .OnGenerateWidget(this, &SOdysseyFolderLayerNodePropertyView::GenerateBlendingComboBoxItem)
                .OnSelectionChanged(this, &SOdysseyFolderLayerNodePropertyView::HandleOnBlendingModeChanged, iFolderLayer, iLayerStack, iFolderNode )
                .Content()
                [
                    //The text in the main button
                    CreateBlendingModeTextWidget( iFolderLayer )
                ]
             ]
        ];


    return finalWidget;
}



int SOdysseyFolderLayerNodePropertyView::GetLayerOpacityValue( FOdysseyFolderLayer* iFolderLayer ) const
{
    return iFolderLayer->GetOpacity() * 100;
}


void SOdysseyFolderLayerNodePropertyView::HandleLayerOpacityValueChanged( int iOpacity, FOdysseyFolderLayer* iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode  )
{
    iFolderLayer->SetOpacity( iOpacity / 100.f );
    iFolderNode->RefreshOpacityText();
}

void SOdysseyFolderLayerNodePropertyView::SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, FOdysseyFolderLayer* iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode  )
{
    iFolderLayer->SetOpacity( iOpacity / 100.f );
    iFolderNode->RefreshOpacityText();
    iLayerStack->ComputeResultBlock();
}


//PRIVATE

void SOdysseyFolderLayerNodePropertyView::HandleOnBlendingModeChanged(TSharedPtr<FText> iNewSelection, ESelectInfo::Type iSelectInfo, FOdysseyFolderLayer* iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode )
{
    iFolderLayer->SetBlendingMode( *(iNewSelection.Get() ) );
    iFolderNode->RefreshBlendingModeText();
    iLayerStack->ComputeResultBlock();

    mBlendingModeComboBox->SetContent(
        SNew(   SComboBox<TSharedPtr<FText>> )
                .OptionsSource(&mBlendingModes)
                .OnGenerateWidget(this, &SOdysseyFolderLayerNodePropertyView::GenerateBlendingComboBoxItem)
                .OnSelectionChanged(this, &SOdysseyFolderLayerNodePropertyView::HandleOnBlendingModeChanged, iFolderLayer, iLayerStack, iFolderNode )
                .Content()
                [
                    //The text in the main button
                    CreateBlendingModeTextWidget( iFolderLayer )
                ]
    );
}



TSharedRef<SWidget> SOdysseyFolderLayerNodePropertyView::GenerateBlendingComboBoxItem(TSharedPtr<FText> iItem)
{
      return SNew(STextBlock)
           .Text(*(iItem.Get()));
}


TSharedRef<SWidget> SOdysseyFolderLayerNodePropertyView::CreateBlendingModeTextWidget( FOdysseyFolderLayer* iFolderLayer)
{
      return SNew(STextBlock)
           .Text( iFolderLayer->GetBlendingModeAsText() );
}




#undef LOCTEXT_NAMESPACE
