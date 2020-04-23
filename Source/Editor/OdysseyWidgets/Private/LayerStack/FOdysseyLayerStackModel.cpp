// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/FOdysseyLayerStackModel.h"

#include "EditorStyleSet.h"

#include "LayerStack/FOdysseyLayerStackTree.h"
#include "LayerStack/SOdysseyLayerStackView.h"

#include "LayerStack/FOdysseyLayerAddMenu.h"

#include "OdysseyLayerStack.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackModel"

//CONSTRUCTOR/DESTRUCTOR

FOdysseyLayerStackModel::FOdysseyLayerStackModel( TSharedPtr<SOdysseyLayerStackView> iWidget, TSharedPtr<FOdysseyLayerStack> iLayerStackData )
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

TSharedRef<FOdysseyLayerStackTree> FOdysseyLayerStackModel::GetNodeTree()
{
    return mNodeTree;
}

TSharedPtr<FOdysseyLayerStack> FOdysseyLayerStackModel::GetLayerStackData()
{
    return mLayerStackData;
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

void FOdysseyLayerStackModel::OnDeleteLayer( IOdysseyLayer* iLayerToDelete )
{
    GetLayerStackData()->DeleteLayer( iLayerToDelete );
    GetLayerStackData()->ComputeResultBlock();
    mLayerStackView->RefreshView();
}

void FOdysseyLayerStackModel::OnMergeLayerDown( IOdysseyLayer* iLayerToMergeDown )
{
    GetLayerStackData()->MergeDownLayer( iLayerToMergeDown );
    mLayerStackView->RefreshView();
}

void FOdysseyLayerStackModel::OnFlattenLayer( IOdysseyLayer* iLayerToMergeDown )
{
    GetLayerStackData()->FlattenLayer( iLayerToMergeDown );
    mLayerStackView->RefreshView();
}

void FOdysseyLayerStackModel::OnDuplicateLayer( IOdysseyLayer* iLayerToDuplicate )
{
    GetLayerStackData()->DuplicateLayer( iLayerToDuplicate );
    mLayerStackView->RefreshView();
}


#undef LOCTEXT_NAMESPACE
