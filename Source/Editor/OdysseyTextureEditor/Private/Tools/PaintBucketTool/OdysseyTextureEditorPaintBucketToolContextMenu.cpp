// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyTextureEditorPaintBucketToolContextMenu.h"
#include "TextureEditor/OdysseyTextureEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorPaintBucketToolContextMenu"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorPaintBucketToolContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorPaintBucketToolContextMenu::~FOdysseyTextureEditorPaintBucketToolContextMenu()
{
}

// https://www.tbwrightartist.com/posts/right_click_context_menu_slate/
FOdysseyTextureEditorPaintBucketToolContextMenu::FOdysseyTextureEditorPaintBucketToolContextMenu(FOdysseyTextureEditor* iEditor)
	: FOdysseyPainterEditorPaintBucketToolContextMenu( static_cast<FOdysseyPainterEditor*>(iEditor) )
{
    mMenu.BeginSection("Context");
    {

    mMenu.AddMenuEntry(
          LOCTEXT("DeleteBucket", "Delete Bucket")
        , LOCTEXT("DeleteBucket", "Delete Bucket")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::DeleteBucket)));
    mMenu.AddMenuEntry(
          LOCTEXT("PropagateBucket", "Propagate Bucket")
        , LOCTEXT("PropagateBucket", "Propagate Bucket")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::PropagateBucket)));
    mMenu.AddMenuEntry(
          LOCTEXT("UnpropagateBucket", "Unpropagate Bucket")
        , LOCTEXT("UnpropagateBucket", "Unpropagate Bucket")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::UnpropagateBucket)));
    }
    mMenu.EndSection();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyTextureEditorPaintBucketToolContextMenu interface

void
FOdysseyTextureEditorPaintBucketToolContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyPainterEditorPaintBucketToolContextMenu::BindShortcuts( iToolkit );
}

/*
void
FOdysseyTextureEditorPaintBucketToolContextMenu::OnToolkitInitialized( FBaseToolkit* iToolkit )
{
    FOdysseyPainterEditorPaintBucketToolContextMenu::OnToolkitInitialized( iToolkit );
}

void
FOdysseyTextureEditorPaintBucketToolContextMenu::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    FOdysseyPainterEditorPaintBucketToolContextMenu::ExtendMenu( iOwner, iMenuName );
}
*/

#undef LOCTEXT_NAMESPACE
