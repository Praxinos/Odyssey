// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPickTool/OdysseyTextureEditorVectorPickToolVertexContextMenu.h"
#include "TextureEditor/OdysseyTextureEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorPickToolVertexContextMenu"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorVectorPickToolVertexContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorVectorPickToolVertexContextMenu::~FOdysseyTextureEditorVectorPickToolVertexContextMenu()
{
}

// https://www.tbwrightartist.com/posts/right_click_context_menu_slate/
FOdysseyTextureEditorVectorPickToolVertexContextMenu::FOdysseyTextureEditorVectorPickToolVertexContextMenu(FOdysseyTextureEditor* iEditor)
	: FOdysseyPainterEditorVectorPickToolVertexContextMenu( static_cast<FOdysseyPainterEditor*>(iEditor) )
{
    mMenu.BeginSection("Context");
    {
    mMenu.AddMenuEntry(
          LOCTEXT("KnotVertices", "Knot Vertices")
        , LOCTEXT("KnotVertices", "Knot Vertices")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateSP(iEditor->GetGUI(), &FOdysseyTextureEditorGUI::KnotVertices)));
    }
    mMenu.EndSection();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyTextureEditorVectorPickToolVertexContextMenu interface

void
FOdysseyTextureEditorVectorPickToolVertexContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyPainterEditorVectorPickToolVertexContextMenu::BindShortcuts( iToolkit );
}

/*
void
FOdysseyTextureEditorVectorPickToolVertexContextMenu::OnToolkitInitialized( FBaseToolkit* iToolkit )
{
    FOdysseyPainterEditorVectorObjectPickToolContextMenu::OnToolkitInitialized( iToolkit );
}

void
FOdysseyTextureEditorVectorPickToolVertexContextMenu::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    FOdysseyPainterEditorVectorObjectPickToolContextMenu::ExtendMenu( iOwner, iMenuName );
}
*/

#undef LOCTEXT_NAMESPACE
