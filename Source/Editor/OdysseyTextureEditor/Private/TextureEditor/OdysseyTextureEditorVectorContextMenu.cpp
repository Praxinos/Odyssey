// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorVectorContextMenu.h"
#include "TextureEditor/OdysseyTextureEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorContextMenu"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorVectorContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorVectorContextMenu::~FOdysseyTextureEditorVectorContextMenu()
{
}

// https://www.tbwrightartist.com/posts/right_click_context_menu_slate/
FOdysseyTextureEditorVectorContextMenu::FOdysseyTextureEditorVectorContextMenu(FOdysseyTextureEditor* iEditor)
	: FOdysseyPainterEditorVectorContextMenu( static_cast<FOdysseyPainterEditor*>(iEditor) )
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
//--------------------------------------------------- FOdysseyTextureEditorVectorContextMenu interface

void
FOdysseyTextureEditorVectorContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyPainterEditorVectorContextMenu::BindShortcuts( iToolkit );
}

/*
void
FOdysseyTextureEditorVectorContextMenu::OnToolkitInitialized( FBaseToolkit* iToolkit )
{
    FOdysseyPainterEditorVectorContextMenu::OnToolkitInitialized( iToolkit );
}

void
FOdysseyTextureEditorVectorContextMenu::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    FOdysseyPainterEditorVectorContextMenu::ExtendMenu( iOwner, iMenuName );
}
*/

#undef LOCTEXT_NAMESPACE
