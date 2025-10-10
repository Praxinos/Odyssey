// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorLayerStackTab.h"

#include "Widgets/Texture/LayerStack/SOdysseyTextureLayerStack.h"
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
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditorTextureSource.h"
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
FOdysseyPainterEditorLayerStackTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    MapActions(toolkitCommands);
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
FOdysseyPainterEditorLayerStackTab::MapActions( TSharedPtr<FUICommandList> iCommandList )
{
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) iCommandList->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorLayerStackTab::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(painterEditorCommands.ImportTexturesAsLayers, ImportTexturesAsLayers )
    MAP_ACTION(painterEditorCommands.ExportLayersAsTextures, ExportLayersAsTextures )
    MAP_ACTION(painterEditorCommands.ExportCurrentLayerAsTexture, ExportCurrentLayerAsTexture )
    MAP_ACTION(painterEditorCommands.ExportTextureToOperatingSystem, ExportTextureToOperatingSystem )

    #undef MAP_ACTION
}

void
FOdysseyPainterEditorLayerStackTab::ExtendMenuFile( TSharedRef<FExtender> iExtender )
{
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
}

void
FOdysseyPainterEditorLayerStackTab::ExportTextureToOperatingSystem()
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);

    UTexture* currentTexture = textureSource->GetTexture();
    IDesktopPlatform* desktopPlatformHandle = FDesktopPlatformModule::Get();
    TArray< FString > filenames;
    bool saveSuccess = desktopPlatformHandle->SaveFileDialog(
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr)
        , LOCTEXT("export-texture-to-os.save-dialog.title", "Select Export Path & Name").ToString()
        , FPaths::ProjectDir()
        , currentTexture->GetName()
        , TEXT("PNG Image (.png)|*.png|BMP Image (.bmp)|*.bmp|TGA Image (.tga)|*.tga|JPG Image (.jpg)|*.jpg")
        , EFileDialogFlags::None
        , filenames
    );

    if (filenames.IsEmpty())
        return;

    FString path = FPaths::GetPath(filenames[0] );
    FString filename = FPaths::GetBaseFilename(filenames[0]);
    path = FPaths::ConvertRelativePathToFull( path );

    Odyssey::ExportAsImage(layerStack, 0, EOdysseyExportImageFormat::PNG, filename, path);
}


void
FOdysseyPainterEditorLayerStackTab::ImportTexturesAsLayers()
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);
    UTexture* currentTexture = textureSource->GetTexture();

    FScopedTransaction ScopedTransaction(LOCTEXT("import-textures-as-layers.transaction.import", "Import Textures As Layers"));

    FOpenAssetDialogConfig openAssetDialogConfig;
    openAssetDialogConfig.DialogTitleOverride = LOCTEXT( "import-textures-as-layers.open-asset-dialog.title", "Import Textures As Layers" );
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(currentTexture->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetClassPathName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(currentTexture->Source.GetFormat());

    if ( assetsData.Num() > 0 )
        layerStack->Modify();

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    for( int i = 0; i < assetsData.Num(); i++ )
    {
        UOdysseyLayer* layer = layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass());
        UOdysseyTextureLayerImageRaster* layerImageRaster = Cast<UOdysseyTextureLayerImageRaster>(layer);
        if ( !layerImageRaster )
            continue;

        UTexture2D* openedTexture = static_cast<UTexture2D*>(assetsData[i].GetAsset());
        ::ULIS::FBlock* textureBlock = NewBlockFromUTextureData(openedTexture, format);

        FOdysseyRasterBlockMutator rasterBlockMutator(layerImageRaster->GetRasterBlock(), false);
        rasterBlockMutator.EditTilesFromRects(
            { textureBlock->Rect() },
            [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<ULIS::FEvent>
            {
                ctx.Copy(*textureBlock, *iBlock);
                ctx.Finish();
                return {};
            }
        );
        rasterBlockMutator.Commit();

    }
}

void
FOdysseyPainterEditorLayerStackTab::ExportLayersAsTextures()
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);
    UTexture* texture = textureSource->GetTexture();

    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "export-layers-as-textures.save-asset-dialog.title", "Export Layers As Texture" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath(texture->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = texture->GetName();
    saveAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetClassPathName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );

    if ( saveObjectPath == "" )
        return;

    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();

    for( UOdysseyLayer* layer : layers )
    {
        if ( layer->CanHaveChildren() ) //avoid exporting folders
            continue;

        FString assetPath = FPaths::GetPath(saveObjectPath) + "/";
        FString textureName = FPaths::GetBaseFilename(saveObjectPath) + TEXT("_") + layer->GetLayerName().ToString().Replace(TEXT(" "), TEXT("_"));
        ::Odyssey::ExportAsTexture(layer, 0, textureName, assetPath );
    }
}

void
FOdysseyPainterEditorLayerStackTab::ExportCurrentLayerAsTexture()
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    if ( !layerStack->GetCurrentLayer() )
        return;

    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);
    UTexture* texture = textureSource->GetTexture();

    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "export-current-layer-as-texture.save-asset-dialog.title", "Export Current Layer As Texture" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath(texture->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = texture->GetName();
    saveAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetClassPathName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );
    if ( saveObjectPath == "" )
        return;

    FString assetPath = FPaths::GetPath(saveObjectPath) + "/";
    FString textureName = FPaths::GetBaseFilename(saveObjectPath) + TEXT("_") + layerStack->GetCurrentLayer()->GetLayerName().ToString().Replace(TEXT(" "), TEXT("_"));
    ::Odyssey::ExportAsTexture(layerStack->GetCurrentLayer(), 0, textureName, assetPath );
}

#undef LOCTEXT_NAMESPACE
