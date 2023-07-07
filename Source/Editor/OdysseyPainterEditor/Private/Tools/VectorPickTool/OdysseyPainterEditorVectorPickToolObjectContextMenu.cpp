// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickToolObjectContextMenu.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorPickToolObjectContextMenu"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorVectorPickToolObjectContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorVectorPickToolObjectContextMenu::~FOdysseyPainterEditorVectorPickToolObjectContextMenu()
{

}

FOdysseyPainterEditorVectorPickToolObjectContextMenu::FOdysseyPainterEditorVectorPickToolObjectContextMenu( FOdysseyPainterEditor* iEditor )
    : FOdysseyPainterEditorContextMenu( iEditor
                                      , TEXT("OdysseyPainterEditor_VectorPickToolObjectContextMenu")
                                      , LOCTEXT( "OdysseyPainterEditorVectorPickToolObjectContextMenu", "Context Menu" )
                                      , FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ) )
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyPainterEditorVectorPickToolObjectContextMenu interface

TSharedPtr<SWidget>
FOdysseyPainterEditorVectorPickToolObjectContextMenu::CreateWidget()
{
    /*
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
    */
    return mMenu.MakeWidget();
}

void
FOdysseyPainterEditorVectorPickToolObjectContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyEditorContextMenu::BindShortcuts(iToolkit);
/*
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorVectorPickToolObjectContextMenu::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
*/
}

/*
void
FOdysseyPainterEditorVectorPickToolObjectContextMenu::OnToolkitInitialized( FBaseToolkit* iToolkit )
{
    FOdysseyEditorContextMenu::OnToolkitInitialized( iToolkit );
}

void
FOdysseyPainterEditorVectorPickToolObjectContextMenu::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    FOdysseyEditorContextMenu::ExtendMenu( iOwner, iMenuName );
}
*/
#undef LOCTEXT_NAMESPACE
