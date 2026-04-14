// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SArianeEditorLayerStackContextMenu.h"
#include "SArianeEditorLayerRow.h"
#include "SArianeEditorLayerStack.h"
#include "ArianeEditor.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

// static
TSharedPtr<SWidget>
SArianeEditorLayerStackContextMenu::CreateWidget( SArianeEditorLayerStack* TreeView )
{
    TSharedPtr<FArianeEditorLayerRowItem> rootItem = TreeView->GetRootItem();
    FArianeEditor* editor = TreeView->GetEditor();
    FMenuBuilder menu( true, nullptr );

    menu.BeginSection("Context");
    {
        menu.AddMenuEntry(
            LOCTEXT("layer-stack-tree-view.context-menu.rename.name", "Rename")
          , LOCTEXT("layer-stack-tree-view.context-menu.rename.tooltip", "Rename")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateRaw(TreeView, &SArianeEditorLayerStack::RenameSelectedItem)));
        menu.AddMenuEntry(
            LOCTEXT("layer-stack-tree-view.context-menu.delete.name", "Delete")
          , LOCTEXT("layer-stack-tree-view.context-menu.delete.tooltip", "Delete")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateRaw(TreeView, &SArianeEditorLayerStack::DeleteSelectedItem)));
    }
    menu.EndSection();

    return menu.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
