// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


// Ariane Editor Headers
#include "SArianeEditorSceneTreeViewContextMenu.h"
#include "SArianeEditorSceneTreeView.h"
#include "SArianeEditorSceneTreeViewRow.h"
#include "ArianeEditor.h"
// Ariane Headers
#include "ArianeObject.h"
#include "ArianePrimitive.h"
#include "ArianeGroup.h"
#include "ArianeImage.h"
#include "ArianeLayerDrawing.h"
// Unreal headers
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Dialogs/Dialogs.h"
// OdysseyVector

#define LOCTEXT_NAMESPACE "PainterEditor"

// static
TSharedPtr<SWidget>
SArianeEditorSceneTreeViewContextMenu::CreateWidget( SArianeEditorSceneTreeView* TreeView )
{
    TSharedPtr<FSceneTreeViewItem> RootItem = TreeView->GetRootItem();
    FArianeGroup* RootGroup = static_cast<FArianeGroup*>(RootItem.Get()->GetObject());
    FArianeEditor* Editor = TreeView->GetEditor();
    FMenuBuilder Menu( true, nullptr );

    Menu.BeginSection("Context");
    {
        Menu.AddMenuEntry(
            LOCTEXT("ariane-scene-tree-view.context-menu.rename.name", "Rename")
          , LOCTEXT("ariane-scene-tree-view.context-menu.rename.tooltip", "Rename")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateRaw(TreeView, &SArianeEditorSceneTreeView::RenameSelectedItem)));

        Menu.AddMenuEntry(
            LOCTEXT("ariane-scene-tree-view.context-menu.group.name", "Group")
          , LOCTEXT("ariane-scene-tree-view.context-menu.group.tooltip", "Group")
          , FSlateIcon()
          , FUIAction( FExecuteAction::CreateStatic( &SArianeEditorSceneTreeViewContextMenu::Group, Editor )
                     , FCanExecuteAction::CreateStatic( &SArianeEditorSceneTreeViewContextMenu::CanGroup, RootGroup )));

        Menu.AddMenuEntry(
            LOCTEXT("ariane-scene-tree-view.context-menu.ungroup.name", "Ungroup")
          , LOCTEXT("ariane-scene-tree-view.context-menu.ungroup.tooltip", "Ungroup")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic( &SArianeEditorSceneTreeViewContextMenu::UngroupSelectedGroups, Editor )
                    , FCanExecuteAction::CreateStatic( &SArianeEditorSceneTreeViewContextMenu::CanUngroup, RootGroup )));

        Menu.AddMenuEntry(
            LOCTEXT("ariane-scene-tree-view.context-menu.copy.name", "Copy")
          , LOCTEXT("ariane-scene-tree-view.context-menu.copy.tooltip", "Copy")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateRaw( Editor, &FArianeEditor::CopySelectedObjects )));

        Menu.AddMenuEntry(
            LOCTEXT("ariane-scene-tree-view.context-menu.paste.name", "Paste")
          , LOCTEXT("ariane-scene-tree-view.context-menu.paste.tooltip", "Paste")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic( &SArianeEditorSceneTreeViewContextMenu::PasteObjects, Editor )
                    , FCanExecuteAction::CreateStatic( &SArianeEditorSceneTreeViewContextMenu::CanPaste, Editor )));

        Menu.AddMenuEntry(
            LOCTEXT("ariane-scene-tree-view.context-menu.delete-selection.name","Delete Selection")
          , LOCTEXT("ariane-scene-tree-view.context-menu.delete-selection.tooltip","Delete Selection")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic( &SArianeEditorSceneTreeViewContextMenu::DeleteSelectedObjects, Editor )
                    , FCanExecuteAction::CreateStatic( &SArianeEditorSceneTreeViewContextMenu::CanDelete, RootGroup )));

        if( HasOnlySelectedPrimitives( RootGroup ) )
        {
             Menu.AddMenuEntry(
                LOCTEXT("ariane-scene-tree-view.context-menu.convert-primitive-to-path.name","Convert primitive to path")
              , LOCTEXT("ariane-scene-tree-view.context-menu.convert-primitive-to-path.tooltip","Convert primitive to path")
              , FSlateIcon()
              , FUIAction(FExecuteAction::CreateRaw( Editor, &FArianeEditor::ConvertSelectedPrimitives ) ));
        }

/*
        menu.AddMenuEntry(
            LOCTEXT("ariane-scene-tree-view.context-menu.copy-transformation.name", "Copy Transformation")
          , LOCTEXT("ariane-scene-tree-view.context-menu.copy-transformation.tooltip", "Copy Transformation")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic(&FArianeEditor::CopyTransformation, vectorScene)));

        menu.AddMenuEntry(
            LOCTEXT("ariane-scene-tree-view.context-menu.paste-transformation.name", "Paste Transformation")
          , LOCTEXT("ariane-scene-tree-view.context-menu.paste-transformation.tooltip", "Paste Transformation")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic(&FArianeEditor::PasteTransformation, iTreeView->GetEditor(), vectorScene)));



        menu.AddMenuEntry(
              LOCTEXT("ariane-tool.object-context-menu.apply-transformations.name", "Apply Transformations")
            , LOCTEXT("ariane-tool.object-context-menu.apply-transformations.tooltip", "Apply Transformations")
            , FSlateIcon()
            , FUIAction(FExecuteAction::CreateStatic( &FArianeEditor::ApplyTransformations, editor, vectorScene )));
*/
    }
    Menu.EndSection();

    return Menu.MakeWidget();
}

// static
void
SArianeEditorSceneTreeViewContextMenu::PasteObjects( FArianeEditor* Editor )
{
    GEditor->BeginTransaction( LOCTEXT("ariane-scene-tree-view.transaction.paste-objects", "Paste Objects") );

    Editor->PasteObjects();

    GEditor->EndTransaction();
}

// static
void
SArianeEditorSceneTreeViewContextMenu::DeleteSelectedObjects( FArianeEditor* Editor )
{
    GEditor->BeginTransaction( LOCTEXT("ariane-scene-tree-view.transaction.delete-selected-objects", "Delete Selected Objects") );

    Editor->DeleteSelectedObjects();

    GEditor->EndTransaction();
}

// static
bool
SArianeEditorSceneTreeViewContextMenu::HasOnlySelectedPrimitives( FArianeGroup* RootGroup )
{
    UArianeLayerDrawing* DrawingLayer = RootGroup->GetImage()->GetDrawingLayer();

    for( FArianeObject* SelectedObject : DrawingLayer->GetImage()->GetSelectedObjects() )
    {
        if( SelectedObject->HasBaseClass( FArianePrimitive::StaticClass() ) == false )
        {
            return false;
        }
    }

    return true;
}

// static
void
SArianeEditorSceneTreeViewContextMenu::UngroupSelectedGroups( FArianeEditor* Editor )
{
    GEditor->BeginTransaction( LOCTEXT("ariane-scene-tree-view.transaction.ungroup-selected-groups", "Ungroup Selected Groups") );

    Editor->UngroupSelectedGroups();

    GEditor->EndTransaction();
}

// static
void
SArianeEditorSceneTreeViewContextMenu::Group( FArianeEditor* Editor )
{
    SGenericDialogWidget::FArguments Args;
/*
    TSharedPtr<SEditableTextBox> TextBox;

    SGenericDialogWidget::OpenDialog( FText::FromString( "test" )
                                    , SAssignNew(TextBox,SEditableTextBox)
                                      .Text( FText::FromString("New Group") )
                                    , Args
                                    , true );

    Editor->GroupSelectedObjects( FName( *TextBox->GetText().ToString() ) );
*/
    GEditor->BeginTransaction( LOCTEXT("ariane-scene-tree-view.transaction.group-selected-object", "Group Selected Objects") );

    Editor->GroupSelectedObjects( "New Group" );

    GEditor->EndTransaction();
}

// static
bool
SArianeEditorSceneTreeViewContextMenu::CanDelete( FArianeGroup* RootGroup )
{
    return RootGroup->IsSelected() ? false : true;
}

// static
bool
SArianeEditorSceneTreeViewContextMenu::CanGroup( FArianeGroup* RootGroup )
{
    return RootGroup->IsSelected() ? false : true;
}

// static
bool
SArianeEditorSceneTreeViewContextMenu::CanUngroup( FArianeGroup* RootGroup )
{
    bool Ret = false;

    for( FArianeObject* SelectedObject : RootGroup->GetImage()->GetSelectedObjects() )
    {
        if( ( SelectedObject->HasBaseClass( FArianeGroup::StaticClass() ) == false )
         || ( SelectedObject == RootGroup ) )
        {
            return false;
        }

        Ret = true;
    }

    return Ret;
}

// static
bool
SArianeEditorSceneTreeViewContextMenu::CanPaste( FArianeEditor* Editor )
{
    return Editor->GetClipboard().CopiedObjects.Num() ? true : false;
}

#undef LOCTEXT_NAMESPACE
