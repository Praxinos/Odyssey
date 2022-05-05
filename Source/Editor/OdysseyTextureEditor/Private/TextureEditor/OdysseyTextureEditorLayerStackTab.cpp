// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorLayerStackTab.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "IContentBrowserSingleton.h"
#include "IDesktopPlatform.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "ToolMenus.h"
#include "OdysseyTextureEditor.h"
#include "ULISLoaderModule.h"
#include "Factories/Texture2dFactoryNew.h"
#include "IOdysseyLayerImageBlendingCapability.h"
#include "OdysseyPixelFormat.h"
#include <ULIS>

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
    mLayerStackView = SNew( SOdysseyLayerStackView )
        .LayerStackData_Raw( this, &FOdysseyTextureEditorLayerStackTab::LayerStack );
    return mLayerStackView;
}

void
FOdysseyTextureEditorLayerStackTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyTextureEditorCommands& textureEditorCommands = FOdysseyTextureEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyTextureEditorLayerStackTab::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(textureEditorCommands.ImportTexturesAsLayers, ImportTexturesAsLayers )
    MAP_ACTION(textureEditorCommands.ExportLayersAsTextures, ExportLayersAsTextures )
    MAP_ACTION(textureEditorCommands.ExportCurrentLayerAsTexture, ExportCurrentLayerAsTexture )
    MAP_ACTION(textureEditorCommands.ExportTextureToOperatingSystem, ExportTextureToOperatingSystem )
    MAP_ACTION(textureEditorCommands.CreateNewLayer, CreateNewLayer )
    MAP_ACTION(textureEditorCommands.DuplicateCurrentLayer, DuplicateCurrentLayer )
    MAP_ACTION(textureEditorCommands.DeleteCurrentLayer, DeleteCurrentLayer )
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

FOdysseyLayerStack*
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
        , TEXT("PNG Image (.png)|*.png|BMP Image (.bmp)|*.bmp|TGA Image (.tga)|*.tga|JPG Image (.jpg)|*.jpg|HDR Image (.HDR)|*.hdr")
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
            CopyUTexturePixelDataIntoBlock( odysseyBlockToSave, currentTexture );
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
    FOpenAssetDialogConfig openAssetDialogConfig;
    openAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ImportTextureDialogTitle", "Import Textures As Layers" );
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(mEditor->Texture()->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );

    for( int i = 0; i < assetsData.Num(); i++ )
    {
        UTexture2D* openedTexture = static_cast<UTexture2D*>( assetsData[i].GetAsset() );
        ::ULIS::FBlock* textureBlock = NewBlockFromUTextureData( openedTexture, mEditor->LayerStack()->Format() );

		FName layerName = mEditor->LayerStack()->GetLayerRoot()->GetNextLayerName();
		TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(FName(*(openedTexture->GetName())), textureBlock));
		mEditor->LayerStack()->AddLayer(imageLayer);
    }

    mLayerStackView->RefreshView();
    mEditor->LayerStack()->ComputeResultInBlock(mEditor->DisplaySurface()->Block());
    mEditor->DisplaySurface()->Invalidate();
}

void           
FOdysseyTextureEditorLayerStackTab::ExportLayersAsTextures()
{
    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ExportLayerDialogTitle", "Export Layers As Texture" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath( mEditor->Texture()->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = mEditor->Texture()->GetName();
    saveAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );

    if( saveObjectPath != "" )
    {
		TArray< TSharedPtr<IOdysseyLayer> > layers;
		mEditor->LayerStack()->GetLayerRoot()->DepthFirstSearchTree(&layers, false);

        for( int i = 0; i < layers.Num(); i++ )
        {
            if( !( layers[i]->GetType() == IOdysseyLayer::eType::kImage ) )
                continue;

            TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer> ( layers[i] );

            FString assetPath = FPaths::GetPath( saveObjectPath ) + "/";
            FString packagePath = ( assetPath + imageLayer->GetName().ToString().Replace( TEXT( " " ), TEXT( "_" ) ) );
            UPackage* package = CreatePackage( *packagePath );

            UTexture2D* object = NewObject<UTexture2D>( package, UTexture2D::StaticClass(), FName( *( FPaths::GetBaseFilename( saveObjectPath ) + TEXT( "_" ) + imageLayer->GetName().ToString().Replace( TEXT( " " ), TEXT( "_" ) ) ) ), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone );
            object->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
            object->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
            object->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
            InitTextureWithBlockData(imageLayer->GetBlock(), object, mEditor->Texture()->Source.GetFormat());

            object->PostEditChange();
            object->UpdateResource();
            object->FinishCachePlatformData(); //Wait UpdateResource Finished

            FAssetRegistryModule::AssetCreated( object );

            UPackage::SavePackage( package, object, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *( imageLayer->GetName().ToString() ) );
            
            package->MarkAsFullyLoaded();
            object->MarkPackageDirty();
        }
    }
}

void           
FOdysseyTextureEditorLayerStackTab::ExportCurrentLayerAsTexture()
{
    TSharedPtr<IOdysseyLayer> layer = mEditor->LayerStack()->GetCurrentLayer();
    if (!layer->ImplementsCapability(IOdysseyLayerImageRenderingCapability::GetGuid()))
        return;

    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked< FAssetToolsModule >("AssetTools").Get();
    UObject* object = AssetTools.CreateAssetWithDialog(UTexture2D::StaticClass(), UTexture2DFactoryNew::StaticClass()->GetDefaultObject<UFactory>());

    if (!object)
        return;

    UTexture2D* texture = Cast<UTexture2D>(object);
    texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    texture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;

    IOdysseyLayerImageRenderingCapability* renderCap = layer->GetCapability<IOdysseyLayerImageRenderingCapability>();
    ::ULIS::FBlock block(mEditor->LayerStack()->Width(), mEditor->LayerStack()->Height(), mEditor->LayerStack()->Format());

    TArray< ::ULIS::FBlock* > blocks;
    TArray< ::ULIS::FRectI > rects;
    TArray< ::ULIS::FVec2I > pos;
    blocks.Add(&block);
    rects.Add(block.Rect());
    pos.Add(::ULIS::FVec2F(0.f, 0.f));
    renderCap->RenderImage(blocks.GetData(), rects.GetData(), pos.GetData(), 1);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block.Format());
    ctx.Finish();

    InitTextureWithBlockData(&block, texture, mEditor->Texture()->Source.GetFormat());

    texture->PostEditChange();
    texture->UpdateResource();

    texture->MarkPackageDirty();
}

void
FOdysseyTextureEditorLayerStackTab::CreateNewLayer()
{
    FName layerName = mEditor->LayerStack()->GetLayerRoot()->GetNextLayerName();
    int w = mEditor->LayerStack()->Width();
    int h = mEditor->LayerStack()->Height();
    ::ULIS::eFormat format = mEditor->LayerStack()->Format();
	TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(layerName, FVector2D(w, h), format));
    mEditor->LayerStack()->AddLayer(imageLayer);
    mEditor->LayerStack()->ComputeResultInBlock(mEditor->DisplaySurface()->Block());
    mEditor->DisplaySurface()->Invalidate();
    mLayerStackView->RefreshView();
}

void
FOdysseyTextureEditorLayerStackTab::DuplicateCurrentLayer()
{
    if( mEditor->LayerStack()->GetCurrentLayer() )
    {
        mEditor->LayerStack()->DuplicateLayer( mEditor->LayerStack()->GetCurrentLayer() );
        mEditor->LayerStack()->ComputeResultInBlock(mEditor->DisplaySurface()->Block());
        mEditor->DisplaySurface()->Invalidate();
        mLayerStackView->RefreshView();
    }
}

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

void
FOdysseyTextureEditorLayerStackTab::ChangeLayerOpacity( float iOpacity )
{
    TSharedPtr<IOdysseyLayer> currentLayer = mEditor->LayerStack()->GetCurrentLayer(); 
    if( currentLayer )
    {
        if( currentLayer->ImplementsCapability( IOdysseyLayerImageBlendingCapability::GetGuid() ) )
        {
            IOdysseyLayerImageBlendingCapability* blendingCapability = currentLayer->GetCapability<IOdysseyLayerImageBlendingCapability>();
            blendingCapability->SetOpacity( FMath::Clamp( iOpacity, 0.f , 1.f ) );
            mLayerStackView->RefreshView();
        }
    }
}

#undef LOCTEXT_NAMESPACE
