// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickToolVertexContextMenu.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorPickToolVertexContextMenu"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorVectorPickToolVertexContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorVectorPickToolVertexContextMenu::~FOdysseyPainterEditorVectorPickToolVertexContextMenu()
{

}

FOdysseyPainterEditorVectorPickToolVertexContextMenu::FOdysseyPainterEditorVectorPickToolVertexContextMenu( FOdysseyPainterEditor* iEditor )
    : FOdysseyPainterEditorContextMenu( iEditor
                                      , TEXT("OdysseyPainterEditor_VectorPickToolVertexContextMenu")
                                      , LOCTEXT( "OdysseyPainterEditorVectorPickToolVertexContextMenu", "Context Menu" )
                                      , FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ) )
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyPainterEditorVectorPickToolVertexContextMenu interface

TSharedPtr<SWidget>
FOdysseyPainterEditorVectorPickToolVertexContextMenu::CreateWidget()
{
    return mMenu.MakeWidget();
}

void
FOdysseyPainterEditorVectorPickToolVertexContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyEditorContextMenu::BindShortcuts(iToolkit);
/*
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorVectorPickToolVertexContextMenu::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
*/
}

/*
void
FOdysseyPainterEditorVectorPickToolVertexContextMenu::OnToolkitInitialized( FBaseToolkit* iToolkit )
{
    FOdysseyEditorContextMenu::OnToolkitInitialized( iToolkit );
}

void
FOdysseyPainterEditorVectorPickToolVertexContextMenu::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    FOdysseyEditorContextMenu::ExtendMenu( iOwner, iMenuName );
}
*/
#undef LOCTEXT_NAMESPACE
