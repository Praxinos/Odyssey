// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#include "Widgets/SArianeEditorLayerDrawingTreeViewContextMenu.h"

#ifdef unused



#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Dialogs/Dialogs.h"

#include "Widgets/Tab/SArianeEditorVectorSceneTreeView.h"
#include "ArianeEditor.h"
// module OdysseyVector
#include "HUD/OdysseyVectorHUD.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "Widgets/Tab/SArianeEditorVectorSceneTreeViewRow.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

// static
TSharedPtr<SWidget>
SArianeEditorVectorSceneTreeViewContextMenu::CreateWidget( SArianeEditorVectorSceneTreeView* iTreeView )
{
    TSharedPtr<FVectorSceneTreeViewItem> rootItem = iTreeView->GetRootItem();
    FOdysseyVectorGroupPaint* vectorScene = static_cast<FOdysseyVectorGroupPaint*>(rootItem.Get()->GetVectorObject());
    FArianeEditor* editor = iTreeView->GetEditor();
    uint64 hudFlags = editor->GetVectorHUDFlags();
    FMenuBuilder menu( true, nullptr );


    menu.BeginSection("Context");
    {
        menu.AddMenuEntry(
            LOCTEXT("vector-scene-tree-view.context-menu.rename.name", "Rename")
          , LOCTEXT("vector-scene-tree-view.context-menu.rename.tooltip", "Rename")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateRaw(iTreeView, &SArianeEditorVectorSceneTreeView::RenameSelectedItem)));

        menu.AddMenuEntry(
            LOCTEXT("vector-scene-tree-view.context-menu.group-paint.name", "Make Paint Group")
          , LOCTEXT("vector-scene-tree-view.context-menu.group-paint.tooltip", "Make Paint Group")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic( &FArianeEditor::MakePaintGroup, iTreeView->GetEditor(), vectorScene )));

        menu.AddMenuEntry(
            LOCTEXT("vector-scene-tree-view.context-menu.group.name", "Group")
          , LOCTEXT("vector-scene-tree-view.context-menu.group.tooltip", "Group")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic( &SArianeEditorVectorSceneTreeViewContextMenu::Group, iTreeView->GetEditor(), vectorScene )));

        menu.AddMenuEntry(
            LOCTEXT("vector-scene-tree-view.context-menu.ungroup.name", "Ungroup")
          , LOCTEXT("vector-scene-tree-view.context-menu.ungroup.tooltip", "Ungroup")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic( &FArianeEditor::Ungroup, iTreeView->GetEditor(), vectorScene )
                    , FCanExecuteAction::CreateStatic( &SArianeEditorVectorSceneTreeViewContextMenu::CanUngroup, vectorScene )));

        menu.AddMenuEntry(
            LOCTEXT("vector-scene-tree-view.context-menu.copy.name", "Copy")
          , LOCTEXT("vector-scene-tree-view.context-menu.copy.tooltip", "Copy")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic(&FArianeEditor::CopyObjects, vectorScene)));

        menu.AddMenuEntry(
            LOCTEXT("vector-scene-tree-view.context-menu.paste.name", "Paste")
          , LOCTEXT("vector-scene-tree-view.context-menu.paste.tooltip", "Paste")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic(&FArianeEditor::PasteObjects, iTreeView->GetEditor(), vectorScene)));

        menu.AddMenuEntry(
            LOCTEXT("vector-scene-tree-view.context-menu.copy-transformation.name", "Copy Transformation")
          , LOCTEXT("vector-scene-tree-view.context-menu.copy-transformation.tooltip", "Copy Transformation")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic(&FArianeEditor::CopyTransformation, vectorScene)));

        menu.AddMenuEntry(
            LOCTEXT("vector-scene-tree-view.context-menu.paste-transformation.name", "Paste Transformation")
          , LOCTEXT("vector-scene-tree-view.context-menu.paste-transformation.tooltip", "Paste Transformation")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic(&FArianeEditor::PasteTransformation, iTreeView->GetEditor(), vectorScene)));

        menu.AddMenuEntry(
            LOCTEXT("vector-scene-tree-view.context-menu.delete-selection.name","Delete Selection")
          , LOCTEXT("vector-scene-tree-view.context-menu.delete-selection.tooltip","Delete Selection")
          , FSlateIcon()
          , FUIAction(FExecuteAction::CreateStatic(&FArianeEditor::DeleteObjects, iTreeView->GetEditor(), vectorScene)));

        if( editor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN_ALLOWED )
        {
            menu.AddMenuEntry(
                LOCTEXT("vector-tool.inbetween-context-menu.groupadd-inbetweener-tag.name", "Group and Add Inbetweener Tag")
              , LOCTEXT("vector-tool.inbetween-context-menu.groupadd-inbetweener-tag.tooltip", "Group and Add Inbetweener Tag")
              , FSlateIcon()
              , FUIAction(FExecuteAction::CreateStatic( &FArianeEditor::GroupAndAddInbetweenerTag, editor, vectorScene )
                        , FCanExecuteAction::CreateStatic( &SArianeEditorVectorSceneTreeViewContextMenu::CanAddInbetweener, vectorScene )));

            menu.AddMenuEntry(
                LOCTEXT("vector-tool.inbetween-context-menu.add-inbetweener-tag.name", "Add Inbetweener Tag")
              , LOCTEXT("vector-tool.inbetween-context-menu.add-inbetweener-tag.tooltip", "Add Inbetweener Tag")
              , FSlateIcon()
              , FUIAction(FExecuteAction::CreateStatic( &FArianeEditor::AddInbetweenerTag, editor, vectorScene )
                        , FCanExecuteAction::CreateStatic( &SArianeEditorVectorSceneTreeViewContextMenu::CanAddInbetweener, vectorScene )));

            menu.AddMenuEntry(
                LOCTEXT("vector-tool.inbetween-context-menu.remove-inbetweener-tag.name", "Remove Inbetweener Tag")
              , LOCTEXT("vector-tool.inbetween-context-menu.remove-inbetweener-tag.tooltip", "Remove Inbetweener Tag")
              , FSlateIcon()
              , FUIAction(FExecuteAction::CreateStatic( &FArianeEditor::RemoveInbetweenerTag, editor, vectorScene)
                        , FCanExecuteAction::CreateStatic( &SArianeEditorVectorSceneTreeViewContextMenu::CanAlterInbetweener, vectorScene )));

            menu.AddMenuEntry(
                LOCTEXT("vector-tool.inbetween-context-menu.commit-inbetweener-tag.name", "Commit Inbetweener Tag")
              , LOCTEXT("vector-tool.inbetween-context-menu.commit-inbetweener-tag.tooltip", "Commit Inbetweener Tag")
              , FSlateIcon()
              , FUIAction(FExecuteAction::CreateStatic( &FArianeEditor::CommitSelectedInbetweenerTag, editor, vectorScene->GetLayer() )
                        , FCanExecuteAction::CreateStatic( &SArianeEditorVectorSceneTreeViewContextMenu::CanAlterInbetweener, vectorScene )));
        }

        menu.AddMenuEntry(
              LOCTEXT("vector-tool.object-context-menu.apply-transformations.name", "Apply Transformations")
            , LOCTEXT("vector-tool.object-context-menu.apply-transformations.tooltip", "Apply Transformations")
            , FSlateIcon()
            , FUIAction(FExecuteAction::CreateStatic( &FArianeEditor::ApplyTransformations, editor, vectorScene )));
    }
    menu.EndSection();

    return menu.MakeWidget();
}

// static
void
SArianeEditorVectorSceneTreeViewContextMenu::Group( FArianeEditor* iEditor
                                                          , FOdysseyVectorGroupPaint* iPaintGroup )
{
    SGenericDialogWidget::FArguments args;
/*
    SGenericDialogWidget::OpenDialog( FText::FromString( "test" )
                                    , SNew(SEditableTextBox)
                                      .Text( FText::FromString("New Group") )
                                    , args
                                    , true );
*/
    FArianeEditor::Group( iEditor, iPaintGroup );
}

// static
bool
SArianeEditorVectorSceneTreeViewContextMenu::CanUngroup( FOdysseyVectorGroupPaint* iScene )
{
    bool ret = false;

    for( FOdysseyVectorObject* selectedObject : iScene->GetCell()->GetSelectedObjectList() )
    {
        if( ( selectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) == false )
         || ( selectedObject == iScene ) )
        {
            return false;
        }

        ret = true;
    }

    return ret;
}

// static
bool
SArianeEditorVectorSceneTreeViewContextMenu::CanAddInbetweener( FOdysseyVectorGroupPaint* iScene )
{
    bool ret = false;

    for( FOdysseyVectorObject* selectedObject : iScene->GetCell()->GetSelectedObjectList() )
    {
        if( selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() ) )
        {
            return false;
        }

        ret = true;
    }

    return ret;
}

// static
bool
SArianeEditorVectorSceneTreeViewContextMenu::CanAlterInbetweener( FOdysseyVectorGroupPaint* iScene )
{
    for( FOdysseyVectorObject* selectedObject : iScene->GetCell()->GetSelectedObjectList() )
    {
        if( selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() ) )
        {
            return true;
        }
    }

    return false;
}

#undef LOCTEXT_NAMESPACE

#endif
