// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeEditorLayerStackTab.h"
#include "SArianeEditorLayerStackPanel.h"
#include "ArianeEditor.h"
#include "ArianeEditorCommands.h"
// Odyssey headers
#include "ArianeEditorStyle.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

const FName&
FArianeEditorLayerStackTab::StaticId()
{
    static FName Id = TEXT("ArianeEditor_LayerStack");

    return Id;
}

/////////////////////////////////////////////////////
// FArianeEditorLayerStackTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FArianeEditorLayerStackTab::~FArianeEditorLayerStackTab()
{
}

FArianeEditorLayerStackTab::FArianeEditorLayerStackTab( FArianeEditor* InEditor )
    : FArianeEditorTab(LOCTEXT( "layerstack-tab.name", "Layer Stack" ), FSlateIcon( "ArianeEditorStyle", "ArianeEditor.Layers16" ))
    , Editor( InEditor )
{
}

const FName&
FArianeEditorLayerStackTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FArianeEditorLayerStackTab::CreateWidget()
{
    return SNew(SArianeEditorLayerStackPanel, Editor );
}

void
FArianeEditorLayerStackTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();

    MapActions(toolkitCommands);
}

void
FArianeEditorLayerStackTab::ExtendMenu( TSharedRef<FExtender> iExtender )
{
    ExtendMenuFile( iExtender );
}

bool
FArianeEditorLayerStackTab::CanOpen() const
{
    return true;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FArianeEditorLayerStackTab::MapActions( TSharedPtr<FUICommandList> iCommandList )
{
    const FArianeEditorCommands& EditorCommands = FArianeEditorCommands::Get();

    #define MAP_ACTION(action, ...) iCommandList->MapAction( action, FExecuteAction::CreateSP( this, &FArianeEditorLayerStackTab::__VA_ARGS__ ), FCanExecuteAction() );
/*
    MAP_ACTION(painterEditorCommands.ImportTexturesAsLayers, ImportTexturesAsLayers )
    MAP_ACTION(painterEditorCommands.ExportLayersAsTextures, ExportLayersAsTextures )
    MAP_ACTION(painterEditorCommands.ExportCurrentLayerAsTexture, ExportCurrentLayerAsTexture )
    MAP_ACTION(painterEditorCommands.ExportTextureToOperatingSystem, ExportTextureToOperatingSystem )
*/
    #undef MAP_ACTION
}

void
FArianeEditorLayerStackTab::ExtendMenuFile( TSharedRef<FExtender> iExtender )
{
/*
    TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
    MapActions(commandList);
    iExtender->AddMenuExtension(
        "OdysseyFile",
        EExtensionHook::After,
        commandList,
        FMenuExtensionDelegate::CreateLambda(
            [this](FMenuBuilder& iBuilder)
            {
                TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
                if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
                    return;

                iBuilder.BeginSection("OdysseyTexture", LOCTEXT("main-menu.file.texture-import-export-section.name", "Texture Import/Export"));
                {
                    iBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().ImportTexturesAsLayers );
                    iBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().ExportLayersAsTextures );
                    iBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().ExportCurrentLayerAsTexture );
                    iBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().ExportTextureToOperatingSystem );
                }
                iBuilder.EndSection();
            }
        )
    );
*/
}

#undef LOCTEXT_NAMESPACE
