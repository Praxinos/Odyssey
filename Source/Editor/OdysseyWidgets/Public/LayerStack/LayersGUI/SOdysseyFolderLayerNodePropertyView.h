// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SComboBox.h"
#include "LayerStack/LayersGUI/OdysseyFolderLayerNode.h"
#include "Misc/NotifyHook.h"
#include "IStructureDetailsView.h"
#include "Framework/SlateDelegates.h"


class FOdysseyFolderLayer;


/**
 * The property view of a layer node
 */
class SOdysseyFolderLayerNodePropertyView : public SCompoundWidget//, public FNotifyHook
{
public:
    ~SOdysseyFolderLayerNodePropertyView();

    SLATE_BEGIN_ARGS(SOdysseyFolderLayerNodePropertyView) {}
        SLATE_EVENT( FOnInt32ValueChanged, OnBlendingModeChanged )
    SLATE_END_ARGS()


    void Construct( const FArguments& InArgs, TSharedRef<IOdysseyBaseLayerNode> iNode );


private:
    TSharedRef<SWidget> ConstructPropertyViewForFolderLayer( FOdysseyFolderLayer* iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode );

    int GetLayerOpacityValue( FOdysseyFolderLayer* iFolderLayer ) const;
    void HandleLayerOpacityValueChanged( int iOpacity, FOdysseyFolderLayer* iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode );
    void SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, FOdysseyFolderLayer* iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode );

private:
    TSharedRef<SWidget> GenerateBlendingComboBoxItem( TSharedPtr<FText> iItem );
    TSharedRef<SWidget> CreateBlendingModeTextWidget( FOdysseyFolderLayer* iFolderLayer );
    void HandleOnBlendingModeChanged(TSharedPtr<FText> iNewSelection, ESelectInfo::Type iSelectInfo, FOdysseyFolderLayer* iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode);


private:
    TSharedPtr<FText> mCurrentBlendingMode;
    TArray< TSharedPtr<FText> > mBlendingModes;

    TSharedPtr<SComboBox<TSharedPtr<FText> > > mBlendingModeComboBox;
};
