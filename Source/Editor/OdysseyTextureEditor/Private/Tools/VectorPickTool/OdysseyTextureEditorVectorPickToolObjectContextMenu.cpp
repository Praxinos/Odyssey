// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPickTool/OdysseyTextureEditorVectorPickToolObjectContextMenu.h"
#include "TextureEditor/OdysseyTextureEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorPickToolObjectContextMenu"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorVectorPickToolObjectContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorVectorPickToolObjectContextMenu::~FOdysseyTextureEditorVectorPickToolObjectContextMenu()
{
}

// https://www.tbwrightartist.com/posts/right_click_context_menu_slate/
FOdysseyTextureEditorVectorPickToolObjectContextMenu::FOdysseyTextureEditorVectorPickToolObjectContextMenu(FOdysseyTextureEditor* iEditor)
	: FOdysseyPainterEditorVectorPickToolObjectContextMenu( static_cast<FOdysseyPainterEditor*>(iEditor) )
{
    mMenu.BeginSection("Context");
    {

    mMenu.AddMenuEntry(
          LOCTEXT("ResetView", "Reset View")
        , LOCTEXT("ResetView", "Reset View")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::ResetView)));
    mMenu.AddMenuEntry(
          LOCTEXT("GroupPaint", "Make Paint Group")
        , LOCTEXT("GroupPaint", "Make Paint Group")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::GroupPaint)));
    mMenu.AddMenuEntry(
          LOCTEXT("Group", "Group")
        , LOCTEXT("Group", "Group")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::Group)));
    mMenu.AddMenuEntry(
          LOCTEXT("Ungroup", "Ungroup")
        , LOCTEXT("Ungroup", "Ungroup")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::Ungroup)));
    mMenu.AddMenuEntry(
          LOCTEXT("BringForward", "Bring forward")
        , LOCTEXT("BringForward", "Bring forward")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::BringForward)));
    mMenu.AddMenuEntry(
          LOCTEXT("SendBackward", "Send backward")
        , LOCTEXT("SendBackward", "Send backward")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::SendBackward)));
    mMenu.AddMenuEntry(
          LOCTEXT("DeleteSelection","Delete Selection")
        , LOCTEXT("DeleteSelection","Delete Selection")
        , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::DeleteSelection)));
    mMenu.AddMenuEntry(
          LOCTEXT("FlipHorizontal","Flip Horizontal")
        , LOCTEXT("FlipHorizontal","Flip Horizontal")
        , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::FlipHorizontal)));
    mMenu.AddMenuEntry(
          LOCTEXT("FlipVertical","Flip Vertical")
        , LOCTEXT("FlipVertical","Flip Vertical")
        , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::FlipVertical)));
    }
    mMenu.EndSection();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyTextureEditorVectorPickToolObjectContextMenu interface

void
FOdysseyTextureEditorVectorPickToolObjectContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyPainterEditorVectorPickToolObjectContextMenu::BindShortcuts( iToolkit );
}

/*
void
FOdysseyTextureEditorVectorPickToolObjectContextMenu::OnToolkitInitialized( FBaseToolkit* iToolkit )
{
    FOdysseyPainterEditorVectorObjectPickToolContextMenu::OnToolkitInitialized( iToolkit );
}

void
FOdysseyTextureEditorVectorPickToolObjectContextMenu::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    FOdysseyPainterEditorVectorObjectPickToolContextMenu::ExtendMenu( iOwner, iMenuName );
}
*/

#undef LOCTEXT_NAMESPACE
