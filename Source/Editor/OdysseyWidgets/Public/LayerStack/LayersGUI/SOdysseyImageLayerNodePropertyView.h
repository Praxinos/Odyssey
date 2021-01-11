// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    TSharedRef<SWidget> ConstructPropertyViewForImageLayer( TSharedPtr<FOdysseyImageLayer> iImageLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyImageLayerNode> iImageNode );

    int GetLayerOpacityValue( TSharedPtr<FOdysseyImageLayer> ImageLayer ) const;
    void HandleLayerOpacityValueChanged( int iOpacity, TSharedPtr<FOdysseyImageLayer> iImageLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyImageLayerNode> iImageNode );
    void SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, TSharedPtr<FOdysseyImageLayer> iImageLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyImageLayerNode> iImageNode );

private:
    TSharedRef<SWidget> GenerateBlendingComboBoxItem( TSharedPtr<FText> iItem );
    TSharedRef<SWidget> CreateBlendingModeTextWidget( TSharedPtr<FOdysseyImageLayer> iImageLayer );
    void HandleOnBlendingModeChanged(TSharedPtr<FText> iNewSelection, ESelectInfo::Type iSelectInfo, TSharedPtr<FOdysseyImageLayer> iImageLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyImageLayerNode> iImageNode);


private:
    TSharedPtr<FText> mCurrentBlendingMode;
    TArray< TSharedPtr<FText> > mBlendingModes;

    TSharedPtr<SComboBox<TSharedPtr<FText> > > mBlendingModeComboBox;
    int mTmpLayerOpacity;
};
