// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SComboBox.h"
#include "LayerStack/LayersGUI/OdysseyImageLayerNode.h"
#include "Misc/NotifyHook.h"
#include "IStructureDetailsView.h"
#include "Framework/SlateDelegates.h"


class FOdysseyImageLayer;
class FOdysseyImageLayerNode;

/**
 * The property view of a layer node
 */
class SOdysseyImageLayerNodePropertyView : public SCompoundWidget
{
public:
    ~SOdysseyImageLayerNodePropertyView();

    SLATE_BEGIN_ARGS(SOdysseyImageLayerNodePropertyView) {}
        SLATE_EVENT( FOnInt32ValueChanged, OnBlendingModeChanged )
    SLATE_END_ARGS()


    void Construct( const FArguments& InArgs, TSharedRef<IOdysseyBaseLayerNode> iNode );


private:
    TSharedRef<SWidget> ConstructPropertyViewForImageLayer( FOdysseyImageLayer* iImageLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyImageLayerNode> iImageNode );

    int GetLayerOpacityValue( FOdysseyImageLayer* ImageLayer ) const;
    void HandleLayerOpacityValueChanged( int iOpacity, FOdysseyImageLayer* iImageLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyImageLayerNode> iImageNode );
    void SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, FOdysseyImageLayer* iImageLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyImageLayerNode> iImageNode );

private:
    TSharedRef<SWidget> GenerateBlendingComboBoxItem( TSharedPtr<FText> iItem );
    TSharedRef<SWidget> CreateBlendingModeTextWidget( FOdysseyImageLayer* iImageLayer );
    void HandleOnBlendingModeChanged(TSharedPtr<FText> iNewSelection, ESelectInfo::Type iSelectInfo, FOdysseyImageLayer* iImageLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyImageLayerNode> iImageNode);


private:
    TSharedPtr<FText> mCurrentBlendingMode;
    TArray< TSharedPtr<FText> > mBlendingModes;

    TSharedPtr<SComboBox<TSharedPtr<FText> > > mBlendingModeComboBox;
};
