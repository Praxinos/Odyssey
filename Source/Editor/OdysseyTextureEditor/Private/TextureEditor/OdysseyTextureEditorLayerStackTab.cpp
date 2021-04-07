// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorLayerStackTab.h"

#include "AssetRegistryModule.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "OdysseyTextureEditor.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorLayerStackTab"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorLayerStackTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorLayerStackTab::~FOdysseyTextureEditorLayerStackTab()
{
}

FOdysseyTextureEditorLayerStackTab::FOdysseyTextureEditorLayerStackTab(FOdysseyTextureEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyTextureEditor_LayerStack"),
                            LOCTEXT( "OdysseyTextureEditorLayerStackTab", "Layer Stack" ),
                            FSlateIcon( "OdysseyStyle", "TextureEditor.Layers16" ))
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
    MAP_ACTION(textureEditorCommands.CreateNewLayer, CreateNewLayer )
    MAP_ACTION(textureEditorCommands.DuplicateCurrentLayer, DuplicateCurrentLayer )
    MAP_ACTION(textureEditorCommands.DeleteCurrentLayer, DeleteCurrentLayer )

    #undef MAP_ACTION
}

void
FOdysseyTextureEditorLayerStackTab::FillExtender(FBaseToolkit* iToolkit, TSharedPtr<FExtender>& ioExtender)
{
    ioExtender->AddMenuExtension(
        "FileLoadAndSave",
        EExtensionHook::After,
		iToolkit->GetToolkitCommands(),
        FMenuExtensionDelegate::CreateRaw( this, &FOdysseyTextureEditorLayerStackTab::ExtendMenuFile ) );
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
FOdysseyTextureEditorLayerStackTab::ExtendMenuFile(FMenuBuilder& ioMenuBuilder)
{
    ioMenuBuilder.BeginSection( "OdysseyTexture", LOCTEXT( "OdysseyTexture", "Odyssey Texture" ) );
    {
        ioMenuBuilder.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ImportTexturesAsLayers );
        ioMenuBuilder.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ExportLayersAsTextures );
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
        FOdysseyBlock* textureBlock = NewOdysseyBlockFromUTextureData( openedTexture, mEditor->LayerStack()->Format() );

		FName layerName = mEditor->LayerStack()->GetLayerRoot()->GetNextLayerName();
		TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(FName(*(openedTexture->GetName())), textureBlock));
		mEditor->LayerStack()->AddLayer(imageLayer);
    }

    mLayerStackView->RefreshView();
    mEditor->LayerStack()->ComputeResultInBlock(mEditor->DisplaySurface()->Block()->GetBlock());
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

            UTexture2D* object = NewObject<UTexture2D>( package, UTexture2D::StaticClass(), FName( *( FPaths::GetBaseFilename( saveObjectPath ) + TEXT( "_" ) + imageLayer->GetName().ToString() ) ), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone );
            object->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
            object->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
            object->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
            InitTextureWithBlockData(imageLayer->GetBlock(), object, mEditor->Texture()->Source.GetFormat());

            object->PostEditChange();
            object->UpdateResource();

            FAssetRegistryModule::AssetCreated( object );

            UPackage::SavePackage( package, object, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *( imageLayer->GetName().ToString() ) );
            
            package->MarkAsFullyLoaded();
            object->MarkPackageDirty();
        }
    }
}

void
FOdysseyTextureEditorLayerStackTab::CreateNewLayer()
{
    FName layerName = mEditor->LayerStack()->GetLayerRoot()->GetNextLayerName();
    int w = mEditor->LayerStack()->Width();
    int h = mEditor->LayerStack()->Height();
    ::ul3::tFormat format = mEditor->LayerStack()->Format();
	TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(layerName, FVector2D(w, h), format));
    mEditor->LayerStack()->AddLayer(imageLayer);
    mEditor->LayerStack()->ComputeResultInBlock(mEditor->DisplaySurface()->Block()->GetBlock());
    mEditor->DisplaySurface()->Invalidate();
    mLayerStackView->RefreshView();
}

void
FOdysseyTextureEditorLayerStackTab::DuplicateCurrentLayer()
{
    if( mEditor->LayerStack()->GetCurrentLayer() )
    {
        mEditor->LayerStack()->DuplicateLayer( mEditor->LayerStack()->GetCurrentLayer() );
        mEditor->LayerStack()->ComputeResultInBlock(mEditor->DisplaySurface()->Block()->GetBlock());
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
            mEditor->LayerStack()->ComputeResultInBlock(mEditor->DisplaySurface()->Block()->GetBlock());
            mEditor->DisplaySurface()->Invalidate();
            mLayerStackView->RefreshView();
        }
    }
}

#undef LOCTEXT_NAMESPACE
