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
