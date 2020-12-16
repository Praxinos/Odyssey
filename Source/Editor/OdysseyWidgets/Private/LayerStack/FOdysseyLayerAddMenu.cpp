// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "LayerStack/FOdysseyLayerAddMenu.h"

#include "LayerStack/FOdysseyLayerStackModel.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "LayerStack/SOdysseyLayerStackTreeView.h"

#include "Framework/MultiBox/MultiBoxBuilder.h" //For FMenuBuilder
#include "EditorStyleSet.h"
#include "OdysseyLayerStack.h"
#include "OdysseyImageLayer.h"


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
    TSharedPtr<IOdysseyLayer> currentLayer = model->GetLayerStackData()->GetCurrentLayer();
	int w = model->GetLayerStackData()->Width();
	int h = model->GetLayerStackData()->Height();
	::ul3::tFormat format = model->GetLayerStackData()->Format();

	FName layerName = model->GetLayerStackData()->GetLayerRoot()->GetNextLayerName();
	TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(layerName, FVector2D(w, h), format));
    if( currentLayer )
    {
        if( currentLayer->GetType() == IOdysseyLayer::eType::kFolder )
        {
            TSharedPtr<FOdysseyFolderLayer> folder = StaticCastSharedPtr<FOdysseyFolderLayer>(currentLayer);
			if (folder->IsOpen())
			{
				model->GetLayerStackData()->AddLayer(imageLayer, currentLayer, 0);
			}
			else
			{
				TSharedPtr<IOdysseyLayer> parent = currentLayer->GetParent();
				model->GetLayerStackData()->AddLayer(imageLayer, parent, currentLayer->GetIndexInParent());
			}
        }
        else
        {
			TSharedPtr<IOdysseyLayer> parent = currentLayer->GetParent();
            model->GetLayerStackData()->AddLayer(imageLayer, parent, currentLayer->GetIndexInParent());
        }
    }
    else
    {
        model->GetLayerStackData()->AddLayer(imageLayer);
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
    TSharedPtr<IOdysseyLayer> currentLayer = model->GetLayerStackData()->GetCurrentLayer();

	FName layerName = model->GetLayerStackData()->GetLayerRoot()->GetNextLayerName();
	TSharedPtr<FOdysseyFolderLayer> folderLayer = MakeShareable(new FOdysseyFolderLayer(layerName));
    if( currentLayer )
    {
        if( currentLayer->GetType() == IOdysseyLayer::eType::kFolder )
        {
            TSharedPtr<FOdysseyFolderLayer> folder = StaticCastSharedPtr<FOdysseyFolderLayer>(currentLayer);
			if (folder->IsOpen())
			{
				model->GetLayerStackData()->AddLayer(folderLayer, currentLayer, 0);
			}
			else
			{
				TSharedPtr<IOdysseyLayer> parent = currentLayer->GetParent();
				model->GetLayerStackData()->AddLayer(folderLayer, parent, currentLayer->GetIndexInParent());
			}
        }
        else
        {
			TSharedPtr<IOdysseyLayer> parent = currentLayer->GetParent();
			model->GetLayerStackData()->AddLayer(folderLayer, parent, currentLayer->GetIndexInParent());
        }
    }
    else
    {
		model->GetLayerStackData()->AddLayer(folderLayer);
    }
    model->GetLayerStackView()->GetTreeView()->Refresh();
}

bool FOdysseyLayerAddMenu::HandleAddFolderLayerMenuEntryCanExecute() const
{
    return true;
}

//---------------------------------------------------------------

#undef LOCTEXT_NAMESPACE

