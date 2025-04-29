// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditorTextureSource.h"
#include "OdysseyPainterEditor.h"
#include "AssetToolsModule.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "OdysseyRasterBlockMutator.h"

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
    return SNew(SOdysseyTextureLayerStack, mEditor)
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

    if( filenames.Num() > 0 )
    {

        FString path( FPaths::ConvertRelativePathToFull( filenames[0] ) );
        std::string str = std::string( TCHAR_TO_UTF8( *path ) );
        std::string extension = std::string( TCHAR_TO_UTF8( *( FPaths::GetExtension( path, false ) ) ) );
        ::ULIS::eFileFormat exportImageFormat = ::ULIS::FileFormat_png;
        bool extensionFound = false;
        for( int i = 0; i <= ::ULIS::FileFormat_hdr; ++i )
        {
            if( extension == ::ULIS::kwImageFormat[i] )
            {
                exportImageFormat = static_cast< ::ULIS::eFileFormat >( i );
                extensionFound = true;
                break;
            }
        }

        if( !extensionFound )
        {
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("export-texture-to-os.invalid-extension-dialog.message", "The file extension or the file format is not supported"), LOCTEXT("export-texture-to-os.invalid-extension-dialog.title", "Invalid extension"));
        }
        else
        {
            FTexturePlatformData* platformData = *currentTexture->GetRunningPlatformData();
            ::ULIS::FBlock* odysseyBlockToSave = new ::ULIS::FBlock( platformData->SizeX, platformData->SizeY, ULISFormatForTextureSourceFormat( currentTexture->Source.GetFormat() ) );
            CopyUTextureSourceDataIntoBlock( odysseyBlockToSave, currentTexture );
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( odysseyBlockToSave->Format() );

            bool canSaveDirectly = false;
            ::ULIS::FContext::SaveBlockToDiskMetrics( *odysseyBlockToSave, exportImageFormat, &canSaveDirectly );
            if (canSaveDirectly)
            {
                ctx.SaveBlockToDisk(
                    *odysseyBlockToSave
                    , str
                    , exportImageFormat
                    , 100
                );

                ctx.Finish();
            }
            else
            {
                ::ULIS::eFormat format = odysseyBlockToSave->Model() == ::ULIS::ColorModel_GREY ? ::ULIS::Format_GA8 : ::ULIS::Format_RGBA8;
                if (exportImageFormat == ::ULIS::FileFormat_hdr)
                {
                    format = ::ULIS::Format_RGBAF;
                }

                ::ULIS::FBlock blockProxy(odysseyBlockToSave->Width(), odysseyBlockToSave->Height(), format);

                ::ULIS::FEvent eventConvert;
                ctx.ConvertFormat(
                    *odysseyBlockToSave
                    , blockProxy
                    , ::ULIS::FRectI::Auto
                    , ::ULIS::FVec2I( 0 )
                    , ULIS::FSchedulePolicy::CacheEfficient
                    , 0
                    , nullptr
                    , &eventConvert
                );

                ctx.SaveBlockToDisk(
                    blockProxy
                    , str
                    , exportImageFormat
                    , 100
                );

                ctx.Finish();
            }

            delete odysseyBlockToSave;
        }
    }
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
    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>(texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    for( UOdysseyLayer* layer : layers )
    {
        if ( layer->CanHaveChildren ) //avoid exporting folders
            continue;

        UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layer);
        if ( !textureLayer )
            continue;

        TSharedPtr<IOdysseyImageRenderer> renderer = textureLayer->BuildImageRenderer(EOdysseyRenderingType::Render, 0);
        renderer->Init();

        FOdysseyImageRendererCopyParams params(block, { ::ULISUtils::ToIntRect(block->Rect()) });
        renderer->Copy(params, {});
        ctx.Finish();

        // Create texture asset
        FString assetPath = FPaths::GetPath(saveObjectPath) + "/";
        FString packagePath = (assetPath + layer->Name.ToString().Replace(TEXT(" "), TEXT("_")));
        UPackage* package = CreatePackage(*packagePath);

        FName textureName(*(FPaths::GetBaseFilename(saveObjectPath) + TEXT("_") + layer->Name.ToString().Replace(TEXT(" "), TEXT("_"))));
        UTexture2D* outTexture = NewObject<UTexture2D>(package, UTexture2D::StaticClass(), textureName, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
        outTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        outTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        outTexture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;

        //can be false on a FX Layer for example
        InitTextureWithBlockData(block.Get(), outTexture, texture->Source.GetFormat());

        outTexture->PostEditChange();
        outTexture->UpdateResource();

        FAssetRegistryModule::AssetCreated(outTexture);

        FSavePackageArgs packageArgs;
        packageArgs.SaveFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
        UPackage::SavePackage( package, outTexture, *( layer->Name.ToString() ), packageArgs );

        package->MarkAsFullyLoaded();
        outTexture->MarkPackageDirty();
    }
}

void
FOdysseyPainterEditorLayerStackTab::ExportCurrentLayerAsTexture()
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    if ( !layerStack->CurrentLayer )
        return;

    UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());
    if ( !textureLayer )
        return;

    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked< FAssetToolsModule >("AssetTools").Get();
    UObject* object = AssetTools.CreateAssetWithDialog(UTexture2D::StaticClass(), UTexture2DFactoryNew::StaticClass()->GetDefaultObject<UFactory>());

    if (!object)
        return;

    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);
    UTexture* texture = textureSource->GetTexture();

    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>(texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    UTexture2D* outTexture = Cast<UTexture2D>(object);
    outTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    outTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    outTexture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;

    TSharedPtr<IOdysseyImageRenderer> renderer = textureLayer->BuildImageRenderer(EOdysseyRenderingType::Render, 0);
    renderer->Init();

    FOdysseyImageRendererCopyParams params(block, { ::ULISUtils::ToIntRect(block->Rect()) });
    renderer->Copy(params, {});
    ctx.Finish();

    InitTextureWithBlockData(block.Get(), outTexture, texture->Source.GetFormat());

    outTexture->PostEditChange();
    outTexture->UpdateResource();
    outTexture->MarkPackageDirty();
}

#undef LOCTEXT_NAMESPACE
