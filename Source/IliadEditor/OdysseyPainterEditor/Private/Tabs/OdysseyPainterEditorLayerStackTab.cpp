// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorLayerStackTab.h"

#include "SOdysseyTextureLayerStack.h"
#include "ToolMenus.h"
#include "DesktopPlatformModule.h"
#include "OdysseyPixelFormat.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "TextureCompiler.h"
#include "ScopedTransaction.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "Factories/Texture2dFactoryNew.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyRasterBlock.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyPainterEditorTextureCommands.h"
#include "OdysseyPainterEditorTextureSource.h"
#include "SOdysseyTextureExportAsImageDialog.h"
#include "SOdysseyTextureExportAsTextureDialog.h"
#include "OdysseyPainterEditor.h"
#include "AssetToolsModule.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyExportImage.h"

#define LOCTEXT_NAMESPACE "TextureEditor"


const FName&
FOdysseyPainterEditorLayerStackTab::StaticId()
{
    static FName Id = TEXT("OdysseyTextureEditor_LayerStack");  //Dont change, Old Id for retro compatibility
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorLayerStackTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorLayerStackTab::~FOdysseyPainterEditorLayerStackTab()
{
}

FOdysseyPainterEditorLayerStackTab::FOdysseyPainterEditorLayerStackTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "layerstack-tab.name", "Layer Stack" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ))
    , mEditor(iEditor)
    , mTextureExportShortcuts(iEditor)
{
}

const FName&
FOdysseyPainterEditorLayerStackTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorLayerStackTab::CreateWidget()
{
    return SNew(SOdysseyTextureLayerStack)
            .LayerStack(this, &FOdysseyPainterEditorLayerStackTab::LayerStack);
}

void
FOdysseyPainterEditorLayerStackTab::ExtendMenu( TSharedRef<FExtender> iExtender )
{
    ExtendMenuFile(iExtender);
}

bool
FOdysseyPainterEditorLayerStackTab::CanOpen() const
{
    //Always display this tab if we use the Odyssey Ed Mode
    if (!mEditor->GetToolkit()->IsAssetEditor())
        return true;

    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    return source && source->Id() == FOdysseyPainterEditorTextureSource::StaticId();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyLayerStack*
FOdysseyPainterEditorLayerStackTab::LayerStack() const
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return nullptr;

    return source->GetLayerStack();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyPainterEditorLayerStackTab::ExtendMenuFile( TSharedRef<FExtender> iExtender )
{
    TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
    mTextureExportShortcuts.MapActionsToCommandList(commandList.ToSharedRef());

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
                    iBuilder.AddSubMenu(
                        LOCTEXT("layerstack-tab.file-menu.import-submenu.name", "Import"),
                        LOCTEXT("layerstack-tab.file-menu.import-submenu.tooltip", "Contains Import actions"),
                        FNewMenuDelegate::CreateRaw(this, &FOdysseyPainterEditorLayerStackTab::BuildImportMenu),
                        false,
                        FSlateIcon( "OdysseyStyle", "AnimationEditor.File-Menu.Import" )
                    );

                    iBuilder.AddSubMenu(
                        LOCTEXT("layerstack-tab.file-menu.export-submenu.name", "Export"),
                        LOCTEXT("layerstack-tab.file-menu.export-submenu.tooltip", "Contains Export actions"),
                        FNewMenuDelegate::CreateRaw(this, &FOdysseyPainterEditorLayerStackTab::BuildExportMenu),
                        false,
                        FSlateIcon( "OdysseyStyle", "AnimationEditor.File-Menu.Export" )
                    );
                }
                iBuilder.EndSection();
            }
        )
    );
}

void
FOdysseyPainterEditorLayerStackTab::BuildImportMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.AddMenuEntry(
        FOdysseyPainterEditorTextureCommands::Get().ImportImages,
        NAME_None,
        LOCTEXT("layerstack-tab.file-menu.import-image.name", "Images...")
    );

    iMenuBuilder.AddMenuEntry(
        FOdysseyPainterEditorTextureCommands::Get().ImportTextures,
        NAME_None,
        LOCTEXT("layerstack-tab.file-menu.import-texture.name", "Textures...")
    );
}

void
FOdysseyPainterEditorLayerStackTab::BuildExportMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.AddMenuEntry(
        FOdysseyPainterEditorTextureCommands::Get().ExportLayersAsImages,
        NAME_None,
        LOCTEXT("layerstack-tab.file-menu.export-image.name", "As Images...")
    );

    iMenuBuilder.AddMenuEntry(
        FOdysseyPainterEditorTextureCommands::Get().ExportLayersAsTextures,
        NAME_None,
        LOCTEXT("layerstack-tab.file-menu.export-texture.name", "As Textures...")
    );
}

#undef LOCTEXT_NAMESPACE
