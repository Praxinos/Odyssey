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


    void Construct( const FArguments& InArgs, TSharedRef<OdysseyBaseLayerNode> Node );


private:
    TSharedRef<SWidget> ConstructPropertyViewForFolderLayer( FOdysseyFolderLayer* FolderLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyFolderLayerNode> trackNode );

    int GetLayerOpacityValue( FOdysseyFolderLayer* FolderLayer ) const;
    void HandleLayerOpacityValueChanged( int iOpacity, FOdysseyFolderLayer* FolderLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyFolderLayerNode> TrackNode );
    void SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, FOdysseyFolderLayer* FolderLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyFolderLayerNode> TrackNode );

private:
    TSharedRef<SWidget> GenerateBlendingComboBoxItem( TSharedPtr<FText> InItem );
    TSharedRef<SWidget> CreateBlendingModeTextWidget( FOdysseyFolderLayer* FolderLayer );
    void HandleOnBlendingModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo, FOdysseyFolderLayer* FolderLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyFolderLayerNode> TrackNode);


private:
    TSharedPtr<FText> CurrentBlendingMode;
    TArray< TSharedPtr<FText> > BlendingModes;

    TSharedPtr<SComboBox<TSharedPtr<FText> > > BlendingModeComboBox;
};
