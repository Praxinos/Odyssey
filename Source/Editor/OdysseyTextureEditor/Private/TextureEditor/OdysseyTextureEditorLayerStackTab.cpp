// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorLayerStackTab.h"

#include "Widgets/LayerStack/SOdysseyTextureLayerStack.h"
#include "ToolMenus.h"
#include "DesktopPlatformModule.h"
#include "OdysseyPixelFormat.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "TextureCompiler.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "Factories/Texture2dFactoryNew.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyRasterBlock.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "TextureEditor/OdysseyTextureEditorCommands.h"
#include "TextureEditor/OdysseyTextureEditorSource.h"
#include "TextureEditor/OdysseyTextureEditorExtension.h"
#include "OdysseyPainterEditor.h"
#include "AssetToolsModule.h"

#define LOCTEXT_NAMESPACE "TextureEditor"


const FName&
FOdysseyTextureEditorLayerStackTab::StaticId()
{
    static FName Id = TEXT("OdysseyTextureEditor_LayerStack"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyTextureEditorLayerStackTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorLayerStackTab::~FOdysseyTextureEditorLayerStackTab()
{
}

FOdysseyTextureEditorLayerStackTab::FOdysseyTextureEditorLayerStackTab(FOdysseyTextureEditorExtension* iExtension)
	: FOdysseyEditorTab(LOCTEXT( "layerstack-tab.name", "Layer Stack" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ))
    , mExtension(iExtension)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyTextureEditorTab interface

const FName&
FOdysseyTextureEditorLayerStackTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyTextureEditorLayerStackTab::CreateWidget()
{
    return SNew(SOdysseyTextureLayerStack)
            .LayerStack(this, &FOdysseyTextureEditorLayerStackTab::LayerStack);
}

void
FOdysseyTextureEditorLayerStackTab::BindShortcuts(FBaseToolkit* iToolkit)
{
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

    #undef MAP_ACTION
}

void
FOdysseyTextureEditorLayerStackTab::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    ExtendMenuFile( iOwner, iMenuName );
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyLayerStack*
FOdysseyTextureEditorLayerStackTab::LayerStack() const
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
        return nullptr;

    return source->GetLayerStack();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyTextureEditorLayerStackTab::ExtendMenuFile( FToolMenuOwner iOwner, FName iMenuName )
{
    UToolMenu* menu = UToolMenus::Get()->FindMenu(*(iMenuName.ToString() + FString(".File")));

    FToolMenuInsert menuInsert;
    if (menu->FindSection("FileActors")) //FileActirs is a weird name but it is the actual name of the "Import/Export" Section from Unreal File Menu
        menuInsert = FToolMenuInsert("FileActors", EToolMenuInsertType::After);

    menu->AddDynamicSection(
        "OdysseyTextureDynamic",
        FNewToolMenuDelegate::CreateLambda(
            [this](UToolMenu* iToolMenu)
            {
                FOdysseyPainterEditor* editor = mExtension->GetEditor();
                if (!editor)
                    return;

                TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
                if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
                    return;
                
                FToolMenuSection& section = iToolMenu->AddSection("OdysseyTexture", LOCTEXT("main-menu.file.texture-import-export-section.name", "Texture Import/Export"));
                {
                    section.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ImportTexturesAsLayers );
                    section.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ExportLayersAsTextures );
                    section.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ExportCurrentLayerAsTexture );
                    section.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ExportTextureToOperatingSystem );
                }
            }
        )
        , menuInsert
    );
}

void
FOdysseyTextureEditorLayerStackTab::ExportTextureToOperatingSystem()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
        return;

    TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(source);

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
FOdysseyTextureEditorLayerStackTab::ImportTexturesAsLayers()
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
        return;

    TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(source);
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
			[&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<ULIS::FEvent>
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
FOdysseyTextureEditorLayerStackTab::ExportLayersAsTextures()
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
        return;

    TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(source);
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

        TSharedPtr<IOdysseyImageRenderer> renderer = textureLayer->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
        renderer->Init();

        FOdysseyImageRendererCopyParams params(block, { block->Rect() });
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
FOdysseyTextureEditorLayerStackTab::ExportCurrentLayerAsTexture()
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

    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
        return;

    TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(source);
    UTexture* texture = textureSource->GetTexture();

    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>(texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    UTexture2D* outTexture = Cast<UTexture2D>(object);
    outTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    outTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    outTexture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;

    TSharedPtr<IOdysseyImageRenderer> renderer = textureLayer->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
    renderer->Init();

    FOdysseyImageRendererCopyParams params(block, { block->Rect() });
    renderer->Copy(params, {});
    ctx.Finish();

    InitTextureWithBlockData(block.Get(), outTexture, texture->Source.GetFormat());

    outTexture->PostEditChange();
    outTexture->UpdateResource();
    outTexture->MarkPackageDirty();
}

void
FOdysseyTextureEditorLayerStackTab::CreateNewLayer()
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    UOdysseyLayer* layer = nullptr;
    {
    #ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("layerstack-tab.transaction.shortcut.create-new-layer", "Add Layer"));
    #endif
        UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
        if (currentLayer)
        {
            if (currentLayer->CanHaveChildren && currentLayer->DisplayChildren)
            {
                layer = layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass(), currentLayer);
            }
            else
            {
                UOdysseyLayer* parent = currentLayer->GetParent();
                int index = currentLayer->GetIndexInParent();
                layer = layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass(), parent, index);
            }
        }
        else
        {
            layer = layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass());
        }

        if (!layer)
            return;
    }
            
    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, "CurrentLayer", TSoftObjectPtr<UOdysseyLayer>(layer));
}

void
FOdysseyTextureEditorLayerStackTab::ChangeLayerOpacity( float iOpacity )
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    if ( !layerStack->CurrentLayer )
        return;
        
    if ( layerStack->CurrentLayer->GetIsLockedRecursively() )
        return;

    if ( !FOdysseyObjectEditorUtils::HasProperty(layerStack->CurrentLayer.Get(), "Opacity") )
        return;


#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("layerstack-tab.transaction.shortcut.set-layer-opacity", "Change Layer Opacity"));
#endif
    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack->CurrentLayer.Get(), "Opacity", FMath::Clamp(iOpacity, 0.f, 1.f));
}

#undef LOCTEXT_NAMESPACE
