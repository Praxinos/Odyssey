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
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateRaw(iTreeView, &SOdysseyPainterEditorVectorSceneTreeView::RenameSelectedItem)));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.group-paint.name", "Make Paint Group")
        , LOCTEXT("vector-scene-tree-view.context-menu.group-paint.tooltip", "Make Paint Group")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::MakePaintGroup, iTreeView->GetEditor(), vectorScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.group.name", "Group")
        , LOCTEXT("vector-scene-tree-view.context-menu.group.tooltip", "Group")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Group, iTreeView->GetEditor(), vectorScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.ungroup.name", "Ungroup")
        , LOCTEXT("vector-scene-tree-view.context-menu.ungroup.tooltip", "Ungroup")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Ungroup, iTreeView->GetEditor(), vectorScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.copy.name", "Copy")
        , LOCTEXT("vector-scene-tree-view.context-menu.copy.tooltip", "Copy")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::CopyObjects, vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.paste.name", "Paste")
        , LOCTEXT("vector-scene-tree-view.context-menu.paste.tooltip", "Paste")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::PasteObjects, iTreeView->GetEditor(), vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.copy-transformation.name", "Copy Transformation")
        , LOCTEXT("vector-scene-tree-view.context-menu.copy-transformation.tooltip", "Copy Transformation")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::CopyTransformation, vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.paste-transformation.name", "Paste Transformation")
        , LOCTEXT("vector-scene-tree-view.context-menu.paste-transformation.tooltip", "Paste Transformation")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::PasteTransformation, iTreeView->GetEditor(), vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("vector-scene-tree-view.context-menu.delete-selection.name","Delete Selection")
        , LOCTEXT("vector-scene-tree-view.context-menu.delete-selection.tooltip","Delete Selection")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteObjects, iTreeView->GetEditor(), vectorScene)));
    }
    menu.EndSection();

    return menu.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
