// IDDN.FR.001.250001.006.S.P.2019.000.00000SOdysseyPainterEditorVectorSceneTreeViewContextMenu
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewContextMenu.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorSceneTreeViewContextMenu"

// static
TSharedPtr<SWidget>
SOdysseyPainterEditorVectorSceneTreeViewContextMenu::CreateWidget( SOdysseyPainterEditorVectorSceneTreeView* iTreeView )
{
    TSharedPtr<FVectorSceneTreeViewItem> rootItem = iTreeView->GetRootItem();
    FOdysseyVectorScene* vectorScene = static_cast<FOdysseyVectorScene*>(rootItem.Get()->GetVectorObject());
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    FMenuBuilder menu( true, nullptr );

    menu.BeginSection("Context");
    {
    menu.AddMenuEntry(
          LOCTEXT("Rename", "Rename")
        , LOCTEXT("Rename", "Rename")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateRaw(iTreeView, &SOdysseyPainterEditorVectorSceneTreeView::RenameSelectedItem)));
    menu.AddMenuEntry(
          LOCTEXT("GroupPaint", "Make Paint Group")
        , LOCTEXT("GroupPaint", "Make Paint Group")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::GroupPaint, vectorEngine, vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("Group", "Group")
        , LOCTEXT("Group", "Group")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Group, vectorEngine, vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("Ungroup", "Ungroup")
        , LOCTEXT("Ungroup", "Ungroup")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Ungroup, vectorEngine, vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("Copy", "Copy")
        , LOCTEXT("Copy", "Copy")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::CopyObjectSelection, vectorEngine, vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("Paste", "Paste")
        , LOCTEXT("Paste", "Paste")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::PasteObjectSelection, vectorEngine, vectorScene)));
    menu.AddMenuEntry(
          LOCTEXT("DeleteSelection","Delete Selection")
        , LOCTEXT("DeleteSelection","Delete Selection")
        , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteObjectSelection, vectorEngine, vectorScene)));
    }
    menu.EndSection();

    return menu.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
