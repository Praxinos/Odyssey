// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/FOdysseyLayerStackModel.h"

#include "EditorStyleSet.h"

#include "LayerStack/FOdysseyLayerStackCommands.h"
#include "LayerStack/FOdysseyLayerStackTree.h"
#include "LayerStack/SOdysseyLayerStackView.h"

#include "LayerStack/FOdysseyLayerAddMenu.h"

#include "OdysseyLayerStack.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackModel"

//CONSTRUCTOR/DESTRUCTOR

FOdysseyLayerStackModel::FOdysseyLayerStackModel( TSharedPtr<SOdysseyLayerStackView> InWidget, TSharedPtr<FOdysseyLayerStack> InLayerStackData )
    : mLayerStackCommandBindings( new FUICommandList )
    , mNodeTree( MakeShareable( new FOdysseyLayerStackTree( *this ) ) )
    , mLayerStackView( InWidget )
    , mLayerStackData( InLayerStackData )
    , mLayerStackAddMenu( MakeShareable( new FOdysseyLayerAddMenu( MakeShareable( this ) ) ) )
{
    FOdysseyLayerStackCommands::Register();
}

FOdysseyLayerStackModel::~FOdysseyLayerStackModel()
{
    FOdysseyLayerStackCommands::Unregister();
}


//PUBLIC API-------------------------------------

void FOdysseyLayerStackModel::BuildAddLayerMenu(FMenuBuilder& MenuBuilder)
{
    mLayerStackAddMenu->BuildAddLayerMenu(MenuBuilder);
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

void FOdysseyLayerStackModel::OnDeleteLayer( IOdysseyLayer* InLayerToDelete )
{
    GetLayerStackData()->DeleteLayer( InLayerToDelete );
    GetLayerStackData()->ComputeResultBlock();
    mLayerStackView->RefreshView();
}

void FOdysseyLayerStackModel::OnMergeLayerDown( IOdysseyLayer* InLayerToMergeDown )
{
    GetLayerStackData()->MergeDownLayer( InLayerToMergeDown );
    mLayerStackView->RefreshView();
}

void FOdysseyLayerStackModel::OnFlattenLayer( IOdysseyLayer* InLayerToMergeDown )
{
    GetLayerStackData()->FlattenLayer( InLayerToMergeDown );
    mLayerStackView->RefreshView();
}

void FOdysseyLayerStackModel::OnDuplicateLayer( IOdysseyLayer* InLayerToDuplicate )
{
    GetLayerStackData()->DuplicateLayer( InLayerToDuplicate );
    mLayerStackView->RefreshView();
}


#undef LOCTEXT_NAMESPACE
