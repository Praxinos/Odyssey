// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SArianeEditorLayerStackContextMenu.h"
#include "SArianeEditorLayerRow.h"
#include "SArianeEditorLayerStack.h"
#include "ArianeEditor.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayer.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerFolder.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

// static
TSharedPtr<SWidget>
SArianeEditorLayerStackContextMenu::CreateWidget( SArianeEditorLayerStack* TreeView )
{
    TSharedPtr<FArianeEditorLayerRowItem> RootItem = TreeView->GetRootItem();
    FArianeEditor* Editor = TreeView->GetEditor();
    FMenuBuilder Menu( true, nullptr );

    Menu.BeginSection("Context");
    {
        Menu.AddMenuEntry(
            LOCTEXT("layer-stack-tree-view.context-menu.rename.name", "Rename")
          , LOCTEXT("layer-stack-tree-view.context-menu.rename.tooltip", "Rename")
          , FSlateIcon()
          , FUIAction( FExecuteAction::CreateRaw(TreeView, &SArianeEditorLayerStack::RenameSelectedItem)));
        Menu.AddMenuEntry(
            LOCTEXT("layer-stack-tree-view.context-menu.delete.name", "Delete")
          , LOCTEXT("layer-stack-tree-view.context-menu.delete.tooltip", "Delete")
          , FSlateIcon()
          , FUIAction( FExecuteAction::CreateRaw( TreeView, &SArianeEditorLayerStack::DeleteSelectedItem )
                     , FCanExecuteAction::CreateStatic( &SArianeEditorLayerStackContextMenu::CanDeleteSelectedLayers, Editor ) ) );
    }
    Menu.EndSection();

    return Menu.MakeWidget();
}

// static
bool
SArianeEditorLayerStackContextMenu::CanDeleteSelectedLayers( FArianeEditor* Editor )
{
    UArianePainting3DComponent* CurrentPainting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( CurrentPainting3DComponent )
    {
        UArianeLayerStack* LayerStack = CurrentPainting3DComponent->GetLayerStack();
        TArray<UArianeLayer*> SelectedTrees;
        uint32 SelectedElderLayerCount = 0;

        LayerStack->GetSelectedTrees( SelectedTrees );

        // check if we are about to delete all top-most layers, which is forbidden (leave at least one)
        for( UArianeLayer* ElderLayer : LayerStack->GetRootFolder()->GetChildLayers() )
        {
            if( SelectedTrees.Contains( ElderLayer ) )
            {
                SelectedElderLayerCount++;
            }
        }

        return ( SelectedElderLayerCount == LayerStack->GetRootFolder()->GetChildLayers().Num() ) ? false
                                                                                                  : true;
    }

    return false;
}

#undef LOCTEXT_NAMESPACE
