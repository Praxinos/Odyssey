// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorTextureExportShortcuts.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorTextureCommands.h"
#include "OdysseyPainterEditorTextureSource.h"
#include "SOdysseyTextureExportAsImageDialog.h"
#include "SOdysseyTextureExportAsTextureDialog.h"
#include "SOdysseyImportTexturesDialog.h"

//Action_ImportTextures()
#include "ScopedTransaction.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "ULISLoaderModule.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyRasterBlockMutator.h"
#include "Factories/TextureFactory.h"

//Action_ImportImages()
#include "OdysseyPainterEditorTextureImport.h"
#include "DesktopPlatformModule.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

FOdysseyPainterEditorTextureExportShortcuts::FOdysseyPainterEditorTextureExportShortcuts(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
}

void
FOdysseyPainterEditorTextureExportShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    FOdysseyEditorShortcuts::MapActionsToCommandList(iCommandList);

    iCommandList->MapAction(
        FOdysseyPainterEditorTextureCommands::Get().ExportLayersAsTextures,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorTextureExportShortcuts::Action_ExportLayersAsTextures)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorTextureCommands::Get().ExportLayersAsImages,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorTextureExportShortcuts::Action_ExportLayersAsImages)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorTextureCommands::Get().ImportImages,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorTextureExportShortcuts::Action_ImportImages)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorTextureCommands::Get().ImportTextures,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorTextureExportShortcuts::Action_ImportTextures)
    );
}

void
FOdysseyPainterEditorTextureExportShortcuts::Action_ExportLayersAsTextures()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);

    UTexture2D* texture = textureSource->GetTexture();

    SOdysseyTextureExportAsTextureDialog::Open(texture);
}

void
FOdysseyPainterEditorTextureExportShortcuts::Action_ExportLayersAsImages()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);

    UTexture2D* texture = textureSource->GetTexture();

    SOdysseyTextureExportAsImageDialog::Open(texture);
}

void
FOdysseyPainterEditorTextureExportShortcuts::Action_ImportImages()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);

    UTexture2D* texture = textureSource->GetTexture();

    IDesktopPlatform* desktopPlatformHandle = FDesktopPlatformModule::Get();
    TArray< FString > filenames;
    bool dialogValidated = desktopPlatformHandle->OpenFileDialog(
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr)
        , LOCTEXT("texture.import-images.dialog.title", "Select Images to import").ToString()
        , FString() //Allows the dialog to open in the last folder the dialog opened
        , texture->GetName()
        , TEXT("PNG Image (.png)|*.png|BMP Image (.bmp)|*.bmp|TGA Image (.tga)|*.tga|JPG Image (.jpg)|*.jpg|Any (.*)|*.*")
        , EFileDialogFlags::Multiple
        , filenames
    );

    if (!dialogValidated || filenames.Num() <= 0)
        return;

    filenames.Sort(
        [](const FString& iA, const FString& iB)
        {
            return iA < iB;
        }
    );

    FOdysseyImportTexturesParameters importParameters;
    importParameters.Init(filenames, texture->GetSurfaceWidth(), texture->GetSurfaceHeight());
    if(!SOdysseyImportTexturesDialog::Open(LOCTEXT("import-images-dialog.title", "Import Images" ), importParameters))
        return;

    FOdysseyPainterEditorTextureImport import;
    TArray<UOdysseyTextureLayerImageRaster*> importedTextures = import.ImportTextures(texture, importParameters);
    if (!importedTextures.IsEmpty())
    {
        UOdysseyLayerStack* layerStack = textureSource->GetLayerStack();
        if (layerStack)
            layerStack->SetCurrentLayer(importedTextures[0]);
    }
}

void
FOdysseyPainterEditorTextureExportShortcuts::Action_ImportTextures()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return;

    UOdysseyLayerStack* layerStack = source->GetLayerStack();
    if ( !layerStack )
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);
    UTexture2D* currentTexture = textureSource->GetTexture();

    FOpenAssetDialogConfig openAssetDialogConfig;
    openAssetDialogConfig.DialogTitleOverride = LOCTEXT( "import-textures-as-layers.open-asset-dialog.title", "Import Textures As Layers" );
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(currentTexture->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetClassPathName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );

    if ( assetsData.IsEmpty() )
        return;

    TArray<UTexture2D*> texturesToImport;
    for( int i = 0; i < assetsData.Num(); i++ )
    {
        UTexture2D* openedTexture = static_cast<UTexture2D*>(assetsData[i].GetAsset());
        texturesToImport.Add(openedTexture);
    }

    FOdysseyImportTexturesParameters importParameters;
    importParameters.Init(texturesToImport, currentTexture->GetSurfaceWidth(), currentTexture->GetSurfaceHeight());
    if(!SOdysseyImportTexturesDialog::Open(LOCTEXT("import-textures-dialog.title", "Import Textures" ), importParameters))
        return;

    FOdysseyPainterEditorTextureImport import;
    TArray<UOdysseyTextureLayerImageRaster*> importedTextures = import.ImportTextures(currentTexture, importParameters);
    if (!importedTextures.IsEmpty())
        layerStack->SetCurrentLayer(importedTextures[0]);
}

#undef LOCTEXT_NAMESPACE
