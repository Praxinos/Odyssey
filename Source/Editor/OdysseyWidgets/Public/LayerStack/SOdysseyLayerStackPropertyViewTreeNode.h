// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SComboBox.h"
#include "LayerStack/LayersGUI/OdysseyTrackLayerNode.h"
#include "Misc/NotifyHook.h"
#include "IStructureDetailsView.h"
#include "Framework/SlateDelegates.h"


class FOdysseyImageLayer;


/**
 * The property view of a layer node
 */
class SOdysseyLayerStackPropertyViewTreeNode : public SCompoundWidget//, public FNotifyHook
{
public:
    ~SOdysseyLayerStackPropertyViewTreeNode();

    SLATE_BEGIN_ARGS(SOdysseyLayerStackPropertyViewTreeNode) {}
        SLATE_EVENT( FOnInt32ValueChanged, OnBlendingModeChanged )
    SLATE_END_ARGS()


    void Construct( const FArguments& InArgs, TSharedRef<OdysseyBaseLayerNode> Node );


private:
    TSharedRef<SWidget> ConstructPropertyViewForImageLayer( FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyTrackLayerNode> trackNode );

    int GetLayerOpacityValue( FOdysseyImageLayer* ImageLayer ) const;
    void HandleLayerOpacityValueChanged( int iOpacity, FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyTrackLayerNode> TrackNode );
    void SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyTrackLayerNode> TrackNode );

private:
    TSharedRef<SWidget> GenerateBlendingComboBoxItem( TSharedPtr<FText> InItem );
    TSharedRef<SWidget> CreateBlendingModeTextWidget( FOdysseyImageLayer* imageLayer );
    void HandleOnBlendingModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo, FOdysseyImageLayer* ImageLayer, FOdysseyLayerStack* LayerStack, TSharedRef<OdysseyTrackLayerNode> TrackNode);


private:
    TSharedPtr<FText> CurrentBlendingMode;
    TArray< TSharedPtr<FText> > BlendingModes;

    TSharedPtr<SComboBox<TSharedPtr<FText> > > BlendingModeComboBox;
};
