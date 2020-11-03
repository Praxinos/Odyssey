// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    TSharedRef<SWidget> ConstructPropertyViewForFolderLayer( TSharedPtr<FOdysseyFolderLayer> iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode );

    int GetLayerOpacityValue( TSharedPtr<FOdysseyFolderLayer> iFolderLayer ) const;
    void HandleLayerOpacityValueChanged( int iOpacity, TSharedPtr<FOdysseyFolderLayer> iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode );
    void SetLayerOpacityValue( int iOpacity, ETextCommit::Type iType, TSharedPtr<FOdysseyFolderLayer> iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode );

private:
    TSharedRef<SWidget> GenerateBlendingComboBoxItem( TSharedPtr<FText> iItem );
    TSharedRef<SWidget> CreateBlendingModeTextWidget( TSharedPtr<FOdysseyFolderLayer> iFolderLayer );
    void HandleOnBlendingModeChanged(TSharedPtr<FText> iNewSelection, ESelectInfo::Type iSelectInfo, TSharedPtr<FOdysseyFolderLayer> iFolderLayer, FOdysseyLayerStack* iLayerStack, TSharedRef<FOdysseyFolderLayerNode> iFolderNode);


private:
    TSharedPtr<FText> mCurrentBlendingMode;
    TArray< TSharedPtr<FText> > mBlendingModes;

    TSharedPtr<SComboBox<TSharedPtr<FText> > > mBlendingModeComboBox;
};
