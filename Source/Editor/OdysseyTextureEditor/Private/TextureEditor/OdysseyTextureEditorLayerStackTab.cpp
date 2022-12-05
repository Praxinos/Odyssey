// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorLayerStackTab.h"

#include "Widgets/Layout/SWidgetSwitcher.h"
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
#include "OdysseySurfaceTexture2DEditable.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorLayerStackTab"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorLayerStackTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorLayerStackTab::~FOdysseyTextureEditorLayerStackTab()
{
}

FOdysseyTextureEditorLayerStackTab::FOdysseyTextureEditorLayerStackTab(FOdysseyTextureEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyTextureEditor_LayerStack")
    , LOCTEXT( "OdysseyTextureEditorLayerStackTab", "Layer Stack" )
    , FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyTextureEditorTab interface

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
    //MAP_ACTION(layerStackEditorCommands.DuplicateCurrentLayer, DuplicateCurrentLayer )
    //MAP_ACTION(layerStackEditorCommands.DeleteCurrentLayer, DeleteCurrentLayer )

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
    return mEditor->LayerStack();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyTextureEditorLayerStackTab::ExtendMenuFile( FToolMenuOwner iOwner, FName iMenuName )
{
    UToolMenu* menu = UToolMenus::Get()->FindMenu(*(iMenuName.ToString() + FString(".File")));

    FToolMenuSection& section = menu->AddSection("OdysseyTexture", LOCTEXT("OdysseyTexture", "Odyssey Texture"), FToolMenuInsert("FileLoadAndSave", EToolMenuInsertType::After));
    {
        section.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ImportTexturesAsLayers );
        section.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ExportLayersAsTextures );
        section.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ExportCurrentLayerAsTexture );
        section.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ExportTextureToOperatingSystem );
    }
}

void
FOdysseyTextureEditorLayerStackTab::ExportTextureToOperatingSystem()
{
    UTexture* currentTexture = mEditor->Texture();
    IDesktopPlatform* desktopPlatformHandle = FDesktopPlatformModule::Get();
    TArray< FString > filenames;
    bool saveSuccess = desktopPlatformHandle->SaveFileDialog(
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr)
        , LOCTEXT("TitleExportTexture", "Select Export Path & Name").ToString()
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
            FText Title = LOCTEXT("TitleExtensionNotFound", "Invalid extension");
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("MessageExtensionNotFound", "The file extension or the file format is not supported"), &Title);
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
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return;

    FOpenAssetDialogConfig openAssetDialogConfig;
    openAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ImportTextureDialogTitle", "Import Textures As Layers" );
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(mEditor->Texture()->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetClassPathName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );

    UTexture* currentTexture = mEditor->Texture();
    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(currentTexture->Source.GetFormat());

    for( int i = 0; i < assetsData.Num(); i++ )
    {
        UOdysseyLayer* layer = layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass());
        UOdysseyTextureLayerImageRaster* layerImageRaster = Cast<UOdysseyTextureLayerImageRaster>(layer);
        if ( !layerImageRaster )
            continue;

        UTexture2D* openedTexture = static_cast<UTexture2D*>(assetsData[i].GetAsset());
        ::ULIS::FBlock* textureBlock = NewBlockFromUTextureData(openedTexture, format);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(textureBlock->Format());

        layerImageRaster->UpdateBlock(*textureBlock, { textureBlock->Rect() }, ::ULIS::FVec2F(0), TArray<::ULIS::FEvent>());
        ctx.Finish();

        delete textureBlock;
    }
}

void           
FOdysseyTextureEditorLayerStackTab::ExportLayersAsTextures()
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return;

    UTexture* texture = mEditor->Texture();

    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ExportLayerDialogTitle", "Export Layers As Texture" );
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
    ::ULIS::FBlock block(texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    for( UOdysseyLayer* layer : layers )
    {
        if ( layer->CanHaveChildren ) //avoid exporting folders
            continue;

        UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layer);
        if ( !textureLayer )
            continue;

        textureLayer->CopyImage(&block, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0, 0), TArray<::ULIS::FEvent>());
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
        InitTextureWithBlockData(&block, outTexture, texture->Source.GetFormat());

        outTexture->PostEditChange();
        outTexture->UpdateResource();
        FTextureCompilingManager::Get().FinishCompilation({ outTexture });

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
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
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

    UTexture* texture = mEditor->Texture();
    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    ::ULIS::FBlock block(texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    UTexture2D* outTexture = Cast<UTexture2D>(object);
    outTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    outTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    outTexture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;

    textureLayer->CopyImage(&block, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0, 0), TArray<::ULIS::FEvent>());
    ctx.Finish();

    InitTextureWithBlockData(&block, outTexture, texture->Source.GetFormat());

    outTexture->PostEditChange();
    outTexture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ outTexture });
    outTexture->MarkPackageDirty();
}

void
FOdysseyTextureEditorLayerStackTab::CreateNewLayer()
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return;

    layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass());
}

/* void
FOdysseyTextureEditorLayerStackTab::DuplicateCurrentLayer()
{
    if( mEditor->LayerStack()->GetCurrentLayer() )
    {
        mEditor->LayerStack()->DuplicateLayer( mEditor->LayerStack()->GetCurrentLayer() );
        mEditor->LayerStack()->ComputeResultInBlock(mEditor->DisplaySurface()->Block());
        mEditor->DisplaySurface()->Invalidate();
        mLayerStackView->RefreshView();
    }
} */

/*
void
FOdysseyTextureEditorLayerStackTab::DeleteCurrentLayer()
{
    if( mEditor->LayerStack()->GetCurrentLayer() )
    {
        FText Title = LOCTEXT("TitleDeletingCurrentLayer", "Deleting current layer");
        if (FMessageDialog::Open(EAppMsgType::OkCancel, LOCTEXT("DeletingCurrentLayer", "Are you sure you want to delete this layer ?"), &Title) == EAppReturnType::Ok)
        {
            mEditor->LayerStack()->DeleteLayer(mEditor->LayerStack()->GetCurrentLayer());
            mEditor->LayerStack()->ComputeResultInBlock(mEditor->DisplaySurface()->Block());
            mEditor->DisplaySurface()->Invalidate();
            mLayerStackView->RefreshView();
        }
    }
}
*/

void
FOdysseyTextureEditorLayerStackTab::ChangeLayerOpacity( float iOpacity )
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return;

    if ( !layerStack->CurrentLayer )
        return;

    if ( !FOdysseyObjectEditorUtils::HasProperty(layerStack->CurrentLayer.Get(), "Opacity") )
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack->CurrentLayer.Get(), "Opacity", FMath::Clamp(iOpacity, 0.f, 1.f));
}

#undef LOCTEXT_NAMESPACE
