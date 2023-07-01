// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectPickTool/OdysseyPainterEditorVectorObjectPickToolContextMenu.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorObjectPickToolContextMenu"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorVectorObjectPickToolContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorVectorObjectPickToolContextMenu::~FOdysseyPainterEditorVectorObjectPickToolContextMenu()
{

}

FOdysseyPainterEditorVectorObjectPickToolContextMenu::FOdysseyPainterEditorVectorObjectPickToolContextMenu( FOdysseyPainterEditor* iEditor )
    : FOdysseyPainterEditorContextMenu( iEditor
                                      , TEXT("OdysseyPainterEditor_VectorObjectPickToolContextMenu")
                                      , LOCTEXT( "OdysseyPainterEditorVectorObjectPickToolContextMenu", "Context Menu" )
                                      , FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ) )
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyPainterEditorVectorObjectPickToolContextMenu interface

TSharedPtr<SWidget>
FOdysseyPainterEditorVectorObjectPickToolContextMenu::CreateWidget()
{
    return mMenu.MakeWidget();
}

void
FOdysseyPainterEditorVectorObjectPickToolContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyEditorContextMenu::BindShortcuts(iToolkit);
/*
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorVectorObjectPickToolContextMenu::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
*/
}

/*
void
FOdysseyPainterEditorVectorObjectPickToolContextMenu::OnToolkitInitialized( FBaseToolkit* iToolkit )
{
    FOdysseyEditorContextMenu::OnToolkitInitialized( iToolkit );
}

void
FOdysseyPainterEditorVectorObjectPickToolContextMenu::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    FOdysseyEditorContextMenu::ExtendMenu( iOwner, iMenuName );
}
*/
#undef LOCTEXT_NAMESPACE
