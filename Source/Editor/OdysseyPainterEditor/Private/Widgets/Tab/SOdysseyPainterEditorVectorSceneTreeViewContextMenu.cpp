// IDDN.FR.001.250001.006.S.P.2019.000.00000SOdysseyPainterEditorVectorSceneTreeViewContextMenu
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewContextMenu.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

// static
TSharedPtr<SWidget>
SOdysseyPainterEditorVectorSceneTreeViewContextMenu::CreateWidget( SOdysseyPainterEditorVectorSceneTreeView* iTreeView )
{
    TSharedPtr<FVectorSceneTreeViewItem> rootItem = iTreeView->GetRootItem();
    FOdysseyVectorGroupPaint* vectorScene = static_cast<FOdysseyVectorGroupPaint*>(rootItem.Get()->GetVectorObject());
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    FMenuBuilder menu( true, nullptr );

    menu.BeginSection("Context");
    {
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.rename.name", "Rename")
        , LOCTEXT("vector-scene-tree-view.context-menu.rename.tooltip", "Rename")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateRaw(iTreeView, &SOdysseyPainterEditorVectorSceneTreeView::RenameSelectedItem)));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.group-paint.name", "Make Paint Group")
        , LOCTEXT("vector-scene-tree-view.context-menu.group-paint.tooltip", "Make Paint Group")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::MakePaintGroup, vectorScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.group.name", "Group")
        , LOCTEXT("vector-scene-tree-view.context-menu.group.tooltip", "Group")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Group, vectorScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.ungroup.name", "Ungroup")
        , LOCTEXT("vector-scene-tree-view.context-menu.ungroup.tooltip", "Ungroup")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Ungroup, vectorScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.copy.name", "Copy")
        , LOCTEXT("vector-scene-tree-view.context-menu.copy.tooltip", "Copy")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::CopyObjects, vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.paste.name", "Paste")
        , LOCTEXT("vector-scene-tree-view.context-menu.paste.tooltip", "Paste")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::PasteObjects, vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.copy-transformation.name", "Copy Transformation")
        , LOCTEXT("vector-scene-tree-view.context-menu.copy-transformation.tooltip", "Copy Transformation")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::CopyTransformation, vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.paste-transformation.name", "Paste Transformation")
        , LOCTEXT("vector-scene-tree-view.context-menu.paste-transformation.tooltip", "Paste Transformation")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::PasteTransformation, vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.delete-selection.name","Delete Selection")
        , LOCTEXT("vector-scene-tree-view.context-menu.delete-selection.tooltip","Delete Selection")
        , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteObjects, vectorScene)));
    }
    menu.EndSection();

    return menu.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
