// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    FOdysseyLayerStack* odysseyLayerStackPtr = iNode->GetLayerStack().GetLayerStackData();
    IOdysseyLayer::eType layerType = iNode->GetLayerDataPtr()->GetType();

    TSharedRef<SWidget> finalWidget = SNullWidget::NullWidget;

    TSharedPtr<FOdysseyFolderLayer> folderLayer = StaticCastSharedPtr<FOdysseyFolderLayer> (iNode->GetLayerDataPtr());
    TSharedRef<FOdysseyFolderLayerNode> folderNode = StaticCastSharedRef<FOdysseyFolderLayerNode>(iNode);
    finalWidget = ConstructPropertyViewForFolderLayer( folderLayer, odysseyLayerStackPtr, folderNode );


    ChildSlot
    [
        finalWidget
    ];
}


//PRIVATE API


TSharedRef<SWidget> SOdysseyFolderLayerNodePropertyView::ConstructPropertyViewForFolderLayer( TSharedPtr<FOdysseyFolderLayer> iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode )
{
    mBlendingModes = iFolderLayer->GetBlendingModesAsText();

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



int SOdysseyFolderLayerNodePropertyView::GetLayerOpacityValue( TSharedPtr<FOdysseyFolderLayer> iFolderLayer ) const
{
    return iFolderLayer->GetOpacity() * 100;
}


void SOdysseyFolderLayerNodePropertyView::HandleLayerOpacityValueChanged( int iOpacity, TSharedPtr<FOdysseyFolderLayer> iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode  )
{
    iFolderLayer->SetOpacity( iOpacity / 100.f );
    iFolderNode->RefreshOpacityText();
}

void SOdysseyFolderLayerNodePropertyView::SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, TSharedPtr<FOdysseyFolderLayer> iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode  )
{
    iFolderLayer->SetOpacity( iOpacity / 100.f );
    iFolderNode->RefreshOpacityText();
}


//PRIVATE

void SOdysseyFolderLayerNodePropertyView::HandleOnBlendingModeChanged(TSharedPtr<FText> iNewSelection, ESelectInfo::Type iSelectInfo, TSharedPtr<FOdysseyFolderLayer> iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode )
{
    iFolderLayer->SetBlendingMode( *(iNewSelection.Get() ) );
    iFolderNode->RefreshBlendingModeText();

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


TSharedRef<SWidget> SOdysseyFolderLayerNodePropertyView::CreateBlendingModeTextWidget( TSharedPtr<FOdysseyFolderLayer> iFolderLayer)
{
      return SNew(STextBlock)
           .Text( iFolderLayer->GetBlendingModeAsText() );
}




#undef LOCTEXT_NAMESPACE
