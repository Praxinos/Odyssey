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

FOdysseyTextureEditorVectorContextMenu::FOdysseyTextureEditorVectorContextMenu(FOdysseyTextureEditor* iEditor)
	: FOdysseyPainterEditorVectorContextMenu( static_cast<FOdysseyPainterEditor*>(iEditor) )
{
    mMenu.AddMenuEntry(
        FOdysseyTextureEditorCommands::Get().ResetView
        , NAME_None
        , LOCTEXT("ResetView", "ResetView")
        , LOCTEXT("ResetView", "ResetView")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16") );
    mMenu.AddMenuEntry(
        FOdysseyTextureEditorCommands::Get().GroupPaint
        , NAME_None
        , LOCTEXT("GroupPaint", "GroupPaint")
        , LOCTEXT("GroupPaint", "GroupPaint")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16") );
    mMenu.AddMenuEntry(
        FOdysseyTextureEditorCommands::Get().Group
        , NAME_None
        , LOCTEXT("Group", "Group")
        , LOCTEXT("Group", "Group")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16") );
    mMenu.AddMenuEntry(
        FOdysseyTextureEditorCommands::Get().Ungroup
        , NAME_None
        , LOCTEXT("Ungroup", "Ungroup")
        , LOCTEXT("Ungroup", "Ungroup")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16") );
    mMenu.AddMenuEntry(
        FOdysseyTextureEditorCommands::Get().BringForward
        , NAME_None
        , LOCTEXT("BringForward", "Bring forward")
        , LOCTEXT("BringForward", "Bring forward")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16") );
    mMenu.AddMenuEntry(
        FOdysseyTextureEditorCommands::Get().SendBackward
        , NAME_None
        , LOCTEXT("SendBackward", "Send backward")
        , LOCTEXT("SendBackward", "Send backward")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16") );
    mMenu.AddMenuEntry(
        FOdysseyTextureEditorCommands::Get().RemoveSelectedObjects
        , NAME_None
        , LOCTEXT("RemoveSelectedObjects","RemoveSelectedObjects")
        , LOCTEXT("RemoveSelectedObjects","RemoveSelectedObjects")
        , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16") );
    mMenu.AddMenuEntry(
        FOdysseyTextureEditorCommands::Get().FlipHorizontal
        , NAME_None
        , LOCTEXT("FlipHorizontal","FlipHorizontal")
        , LOCTEXT("FlipHorizontal","FlipHorizontal")
        , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16") );
    mMenu.AddMenuEntry(
        FOdysseyTextureEditorCommands::Get().FlipVertical
        , NAME_None
        , LOCTEXT("FlipVertical","FlipVertical")
        , LOCTEXT("FlipVertical","FlipVertical")
        , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16") );
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyTextureEditorVectorContextMenu interface

void
FOdysseyTextureEditorVectorContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
/*
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyTextureEditorCommands& textureEditorCommands = FOdysseyTextureEditorCommands::Get();
    //const FOdysseyLayerStackEditorCommands& layerStackEditorCommands = FOdysseyLayerStackEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyTextureEditorLayerStackTab::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(textureEditorCommands.ImportTexturesAsLayers, ImportTexturesAsLayers )
    MAP_ACTION(textureEditorCommands.ExportLayersAsTextures, ExportLayersAsTextures )
    MAP_ACTION(textureEditorCommands.ExportCurrentLayerAsTexture, ExportCurrentLayerAsTexture )
    MAP_ACTION(textureEditorCommands.ExportTextureToOperatingSystem, ExportTextureToOperatingSystem )
    MAP_ACTION(textureEditorCommands.CreateNewLayer, CreateNewLayer )
    MAP_ACTION(textureEditorCommands.ChangeLayerOpacity10, ChangeLayerOpacity, 0.1f )
    MAP_ACTION(textureEditorCommands.ChangeLayerOpacity20, ChangeLayerOpacity, 0.2f )
    MAP_ACTION(textureEditorCommands.ChangeLayerOpacity30, ChangeLayerOpacity, 0.3f )
    MAP_ACTION(textureEditorCommands.ChangeLayerOpacity40, ChangeLayerOpacity, 0.4f )
    MAP_ACTION(textureEditorCommands.ChangeLayerOpacity50, ChangeLayerOpacity, 0.5f )
    MAP_ACTION(textureEditorCommands.ChangeLayerOpacity60, ChangeLayerOpacity, 0.6f )
    MAP_ACTION(textureEditorCommands.ChangeLayerOpacity70, ChangeLayerOpacity, 0.7f )
    MAP_ACTION(textureEditorCommands.ChangeLayerOpacity80, ChangeLayerOpacity, 0.8f )
    MAP_ACTION(textureEditorCommands.ChangeLayerOpacity90, ChangeLayerOpacity, 0.9f )
    MAP_ACTION(textureEditorCommands.ChangeLayerOpacity100, ChangeLayerOpacity, 1.0f )
    //MAP_ACTION(layerStackEditorCommands.DuplicateCurrentLayer, DuplicateCurrentLayer )
    //MAP_ACTION(layerStackEditorCommands.DeleteCurrentLayer, DeleteCurrentLayer )

    #undef MAP_ACTION
*/
}

#undef LOCTEXT_NAMESPACE
