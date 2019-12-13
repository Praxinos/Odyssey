// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/FOdysseyLayerStackModel.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Textures/SlateIcon.h"
#include "EditorStyleSet.h"

#include "Widgets/Input/SButton.h"

#include "LayerStack/FOdysseyLayerStackCommands.h"
#include "LayerStack/FOdysseyLayerStackTree.h"
#include "LayerStack/SOdysseyLayerStackView.h"

#include "LayerStack/LayersEditor/IOdysseyLayerEditor.h"
#include "LayerStack/LayersEditor/FOdysseyImageLayerEditor.h" //TODO: Abstract this so we won't have to include all the layers Editors Manually

#include "OdysseyLayerStack.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackModel"

//CONSTRUCTOR/DESTRUCTOR

FOdysseyLayerStackModel::FOdysseyLayerStackModel( TSharedPtr<SOdysseyLayerStackView> InWidget, TSharedPtr<FOdysseyLayerStack> InLayerStackData )
    : LayerStackCommandBindings( new FUICommandList )
    , LayerStackSharedBindings( new FUICommandList )
    , NodeTree( MakeShareable( new FOdysseyLayerStackTree( *this ) ) )
    , LayerStackWidget( InWidget )
    , LayerStackData( InLayerStackData )
    , LayerEditors( TArray<TSharedPtr<IOdysseyLayerEditor>>() )
    , bIsOnlyCurrentVisibleLayer(false)
{
    FOdysseyLayerStackCommands::Register();
    //UE_LOG(LogTemp, Display, TEXT("Number in layer stack data: %d"), LayerStackData->GetLayers()->Num())

    BindCommands();

    LayerEditors.Add(FOdysseyImageLayerEditor::CreateLayerEditor( MakeShareable(this) ));
}

FOdysseyLayerStackModel::~FOdysseyLayerStackModel()
{
    FOdysseyLayerStackCommands::Unregister();
}


//PUBLIC API-------------------------------------

void FOdysseyLayerStackModel::BuildAddLayerMenu(FMenuBuilder& MenuBuilder)
{
    for (int32 i = 0; i < LayerEditors.Num(); ++i)
    {
        LayerEditors[i]->BuildAddLayerMenu(MenuBuilder);
    }
}


TSharedRef<FOdysseyLayerStackTree> FOdysseyLayerStackModel::GetNodeTree()
{
    return NodeTree;
}

TSharedPtr<FOdysseyLayerStack> FOdysseyLayerStackModel::GetLayerStackData()
{
    return LayerStackData;
}


TSharedRef<SOdysseyLayerStackView> FOdysseyLayerStackModel::GetLayerStackWidget() const
{
    return LayerStackWidget.ToSharedRef();
}


//PROTECTED API----------------------------------

void FOdysseyLayerStackModel::BindCommands()
{
    const FOdysseyLayerStackCommands& Commands = FOdysseyLayerStackCommands::Get();

    LayerStackCommandBindings->MapAction(
        Commands.TestOption,
        FExecuteAction::CreateRaw( this, &FOdysseyLayerStackModel::TestOption ) );
}

void FOdysseyLayerStackModel::TestOption()
{
    bIsOnlyCurrentVisibleLayer = !bIsOnlyCurrentVisibleLayer;

    if( bIsOnlyCurrentVisibleLayer )
    {
        UE_LOG(LogTemp, Display, TEXT("Animation mode activated"));
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Animation mode deactivated"));
    }

}

void FOdysseyLayerStackModel::OnAddFolder()
{
}

void FOdysseyLayerStackModel::OnAddLayer(const IOdysseyLayer& InLayer)
{
}


void FOdysseyLayerStackModel::OnDeleteLayer( IOdysseyLayer* InLayerToDelete )
{
    GetLayerStackData()->DeleteLayer( InLayerToDelete );
    GetLayerStackData()->ComputeResultBlock();
    LayerStackWidget->RefreshView();
}

void FOdysseyLayerStackModel::OnMergeLayerDown( IOdysseyLayer* InLayerToMergeDown )
{
    GetLayerStackData()->MergeDownLayer( InLayerToMergeDown );
    LayerStackWidget->RefreshView();
}

void FOdysseyLayerStackModel::OnDuplicateLayer( IOdysseyLayer* InLayerToDuplicate )
{
    GetLayerStackData()->DuplicateLayer( InLayerToDuplicate );
    LayerStackWidget->RefreshView();
}


#undef LOCTEXT_NAMESPACE
