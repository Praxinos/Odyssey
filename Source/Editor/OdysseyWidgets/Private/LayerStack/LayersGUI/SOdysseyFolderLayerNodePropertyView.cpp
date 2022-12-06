// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/LayersGUI/SOdysseyFolderLayerNodePropertyView.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBox.h"
#include "OdysseyFolderLayer.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"


#define LOCTEXT_NAMESPACE "SOdysseyFolderLayerNodePropertyView"

//CONSTRUCTION/DESTRUCTION--------------------------------------

SOdysseyFolderLayerNodePropertyView::~SOdysseyFolderLayerNodePropertyView()
{
}



void SOdysseyFolderLayerNodePropertyView::Construct( const FArguments& InArgs, TSharedRef<IOdysseyBaseLayerNode> iNode )
{
    mTmpFolderOpacity = -1;

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
        .AutoHeight()
        [
            SNew(SHorizontalBox )
            + SHorizontalBox::Slot()
            .Padding(FMargin(0.f, 0.f, 4.f, 0.f))
            .FillWidth(0.5f)
            [
                SNew( STextBlock )
                .Text(LOCTEXT("Opacity", "Opacity"))
            ]

            +SHorizontalBox::Slot()
            .FillWidth(0.5f)
            .VAlign( VAlign_Center )
            [
                SNew(SSpinBox<int>)
                //.Style( FAppStyle::Get(), "NoBorder" )
                .Value(this, &SOdysseyFolderLayerNodePropertyView::GetLayerOpacityValue, iFolderLayer)
                .Delta(1)
                .LinearDeltaSensitivity( 15 )
                .TypeInterface(MakeShared<TNumericUnitTypeInterface<int>>(EUnit::Percentage))
                .OnValueChanged(this, &SOdysseyFolderLayerNodePropertyView::HandleLayerOpacityValueChanged, iFolderLayer, iLayerStack, iFolderNode )
                .OnValueCommitted(this, &SOdysseyFolderLayerNodePropertyView::SetLayerOpacityValue, iFolderLayer, iLayerStack, iFolderNode )
            ]
        ]
        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 3.f, 0.f, 0.f))
        .AutoHeight()
        [
            SNew(SHorizontalBox )
            + SHorizontalBox::Slot()
            .Padding(FMargin(0.f, 0.f, 4.f, 0.f))
            .FillWidth(0.5f)
            [
                SNew( STextBlock )
                .Text(LOCTEXT("Blending Mode", "Blending Mode"))
            ]

            + SHorizontalBox::Slot()
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
    return mTmpFolderOpacity >= 0.0f ? mTmpFolderOpacity : iFolderLayer->GetOpacity() * 100;
}


void SOdysseyFolderLayerNodePropertyView::HandleLayerOpacityValueChanged( int iOpacity, TSharedPtr<FOdysseyFolderLayer> iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode  )
{
    mTmpFolderOpacity = FMath::Clamp( iOpacity, 0, 100 );
}

void SOdysseyFolderLayerNodePropertyView::SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, TSharedPtr<FOdysseyFolderLayer> iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode  )
{
    iFolderLayer->SetOpacity( FMath::Clamp( iOpacity, 0, 100 ) / 100.f );
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
