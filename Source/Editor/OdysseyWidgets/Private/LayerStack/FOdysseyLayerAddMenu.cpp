// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/FOdysseyLayerAddMenu.h"

#include "LayerStack/FOdysseyLayerStackModel.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "LayerStack/SOdysseyLayerStackTreeView.h"

#include "Framework/MultiBox/MultiBoxBuilder.h" //For FMenuBuilder
#include "EditorStyleSet.h"
#include "OdysseyLayerStack.h"


#define LOCTEXT_NAMESPACE "FOdysseyLayerAddMenu"

//Construction

FOdysseyLayerAddMenu::FOdysseyLayerAddMenu( TSharedRef<FOdysseyLayerStackModel> iLayerStack )
    : mLayerStackRef(iLayerStack)
{
}


//Implementation of FOdysseyLayerAddMenu--------------------------

void FOdysseyLayerAddMenu::BuildAddLayerMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.AddMenuEntry(
        LOCTEXT("AddImageLayer", "Add Image Layer"),
        LOCTEXT("AddImageLayerTooltip", "Adds a new image layer"),
        FSlateIcon(FEditorStyle::GetStyleSetName(), ""),
        FUIAction(
            FExecuteAction::CreateRaw(this, &FOdysseyLayerAddMenu::HandleAddImageLayerMenuEntryExecute)
        )
    );
    
    iMenuBuilder.AddMenuEntry(
        LOCTEXT("AddFolder", "Add Folder Layer"),
        LOCTEXT("AddFolderTooltip", "Adds a new folder layer"),
        FSlateIcon(FEditorStyle::GetStyleSetName(), ""),
        FUIAction(
            FExecuteAction::CreateRaw(this, &FOdysseyLayerAddMenu::HandleAddFolderLayerMenuEntryExecute)
        )
    );
}

//Callbacks------------------------------------------------------

void FOdysseyLayerAddMenu::HandleAddImageLayerMenuEntryExecute()
{
    FOdysseyLayerStackModel* model = static_cast<FOdysseyLayerStackModel*>(&mLayerStackRef.Get());
    FOdysseyNTree<IOdysseyLayer*>* currentNode = model->GetLayerStackData()->GetCurrentLayer();
    
    if( currentNode )
    {
        IOdysseyLayer* currentLayer = currentNode->GetNodeContent();

        if( currentLayer->GetType() == IOdysseyLayer::eType::kFolder )
        {
            FOdysseyFolderLayer* folder = static_cast<FOdysseyFolderLayer*>(currentLayer);
            if( folder->IsOpen() )
                model->GetLayerStackData()->AddImageLayer( model->GetLayerStackData()->GetCurrentLayer(), 0 );
            else
                model->GetLayerStackData()->AddImageLayer( model->GetLayerStackData()->GetCurrentLayer()->GetParent(), model->GetLayerStackData()->GetCurrentLayer()->GetIndexInParent() );

        }
        else
        {
            model->GetLayerStackData()->AddImageLayer( model->GetLayerStackData()->GetCurrentLayer()->GetParent(), model->GetLayerStackData()->GetCurrentLayer()->GetIndexInParent() );
        }
    }
    else
    {
        model->GetLayerStackData()->AddImageLayer();
    }
    model->GetLayerStackView()->GetTreeView()->Refresh();
}

bool FOdysseyLayerAddMenu::HandleAddImageLayerMenuEntryCanExecute() const
{
    return true;
}

void FOdysseyLayerAddMenu::HandleAddFolderLayerMenuEntryExecute()
{
    FOdysseyLayerStackModel* model = static_cast<FOdysseyLayerStackModel*>(&mLayerStackRef.Get());
    FOdysseyNTree< IOdysseyLayer* >* currentLayerNode = model->GetLayerStackData()->GetCurrentLayer();
    if( currentLayerNode )
    {
        IOdysseyLayer* currentLayer = currentLayerNode->GetNodeContent();
        if( currentLayer->GetType() == IOdysseyLayer::eType::kFolder )
        {
            FOdysseyFolderLayer* folder = static_cast<FOdysseyFolderLayer*>(currentLayer);
            if( folder->IsOpen() )
                model->GetLayerStackData()->AddFolderLayer( model->GetLayerStackData()->GetCurrentLayer(), FName(), 0 );
            else
                model->GetLayerStackData()->AddFolderLayer( model->GetLayerStackData()->GetCurrentLayer()->GetParent(), FName(), model->GetLayerStackData()->GetCurrentLayer()->GetIndexInParent() );

        }
        else
        {
            model->GetLayerStackData()->AddFolderLayer( model->GetLayerStackData()->GetCurrentLayer()->GetParent(), FName(), model->GetLayerStackData()->GetCurrentLayer()->GetIndexInParent() );
        }
    }
    else
    {
        model->GetLayerStackData()->AddFolderLayer();
    }
    model->GetLayerStackView()->GetTreeView()->Refresh();
}

bool FOdysseyLayerAddMenu::HandleAddFolderLayerMenuEntryCanExecute() const
{
    return true;
}

//---------------------------------------------------------------

#undef LOCTEXT_NAMESPACE

