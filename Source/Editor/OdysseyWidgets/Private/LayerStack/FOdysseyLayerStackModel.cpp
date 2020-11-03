// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "LayerStack/FOdysseyLayerStackModel.h"

#include "EditorStyleSet.h"

#include "LayerStack/FOdysseyLayerStackTree.h"
#include "LayerStack/SOdysseyLayerStackView.h"

#include "LayerStack/FOdysseyLayerAddMenu.h"

#include "OdysseyLayerStack.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackModel"

//CONSTRUCTOR/DESTRUCTOR

FOdysseyLayerStackModel::FOdysseyLayerStackModel( TSharedPtr<SOdysseyLayerStackView> iWidget, const TAttribute< FOdysseyLayerStack* >& iLayerStackData )
    : mLayerStackCommandBindings( new FUICommandList )
    , mNodeTree( MakeShareable( new FOdysseyLayerStackTree( *this ) ) )
    , mLayerStackView( iWidget )
    , mLayerStackData( iLayerStackData )
    , mLayerStackAddMenu( MakeShareable( new FOdysseyLayerAddMenu( MakeShareable( this ) ) ) )
{
}

FOdysseyLayerStackModel::~FOdysseyLayerStackModel()
{
}


//PUBLIC API-------------------------------------

void FOdysseyLayerStackModel::BuildAddLayerMenu(FMenuBuilder& iMenuBuilder)
{
    mLayerStackAddMenu->BuildAddLayerMenu(iMenuBuilder);
}

TSharedPtr<FOdysseyLayerStackTree> FOdysseyLayerStackModel::GetNodeTree() const
{
    return mNodeTree;
}

FOdysseyLayerStack* FOdysseyLayerStackModel::GetLayerStackData()
{
    return mLayerStackData.Get();
}

const TSharedRef< FOdysseyLayerAddMenu > FOdysseyLayerStackModel::GetLayerAddMenu() const
{
    return mLayerStackAddMenu;
}

TSharedPtr<FUICommandList> FOdysseyLayerStackModel::GetCommandBindings() const
{
    return mLayerStackCommandBindings;
}

TSharedRef<SOdysseyLayerStackView> FOdysseyLayerStackModel::GetLayerStackView() const
{
    return mLayerStackView.ToSharedRef();
}


//HANDLES ----------------------------------

void FOdysseyLayerStackModel::OnDeleteLayer( TSharedPtr<IOdysseyLayer> iLayerToDelete )
{
    GetLayerStackData()->DeleteLayer( iLayerToDelete );
    mLayerStackView->RefreshView();
}

void FOdysseyLayerStackModel::OnMergeLayerDown(TSharedPtr<IOdysseyLayer> iLayerToMergeDown )
{
    GetLayerStackData()->MergeDownLayer( iLayerToMergeDown );
    mLayerStackView->RefreshView();
}

void FOdysseyLayerStackModel::OnFlattenLayer(TSharedPtr<IOdysseyLayer> iLayerToMergeDown )
{
    GetLayerStackData()->FlattenLayer( iLayerToMergeDown );
    mLayerStackView->RefreshView();
}

void FOdysseyLayerStackModel::OnDuplicateLayer(TSharedPtr<IOdysseyLayer> iLayerToDuplicate )
{
    GetLayerStackData()->DuplicateLayer( iLayerToDuplicate );
    mLayerStackView->RefreshView();
}


#undef LOCTEXT_NAMESPACE
