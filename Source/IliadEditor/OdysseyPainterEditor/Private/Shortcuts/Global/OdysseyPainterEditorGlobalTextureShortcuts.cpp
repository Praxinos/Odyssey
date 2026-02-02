// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorGlobalTextureShortcuts.h"

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

FOdysseyPainterEditorGlobalTextureShortcuts::FOdysseyPainterEditorGlobalTextureShortcuts(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
}

void
FOdysseyPainterEditorGlobalTextureShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    FOdysseyEditorShortcuts::MapActionsToCommandList(iCommandList);

    iCommandList->MapAction(
        FOdysseyPainterEditorTextureCommands::Get().ExportLayersAsTextures,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTextureShortcuts::Action_ExportLayersAsTextures)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorTextureCommands::Get().ExportLayersAsImages,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTextureShortcuts::Action_ExportLayersAsImages)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorTextureCommands::Get().ImportImages,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTextureShortcuts::Action_ImportImages)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorTextureCommands::Get().ImportTextures,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTextureShortcuts::Action_ImportTextures)
    );
}

void
FOdysseyPainterEditorGlobalTextureShortcuts::Action_ExportLayersAsTextures()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);

    UTexture2D* texture = textureSource->GetTexture();

    SOdysseyTextureExportAsTextureDialog::Open(texture);
}

void
FOdysseyPainterEditorGlobalTextureShortcuts::Action_ExportLayersAsImages()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(source);

    UTexture2D* texture = textureSource->GetTexture();

    SOdysseyTextureExportAsImageDialog::Open(texture);
}

void
FOdysseyPainterEditorGlobalTextureShortcuts::Action_ImportImages()
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
        , FPaths::ProjectDir()
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

    //Convert to textures
    FScopedSlowTask progressBar(filenames.Num(), LOCTEXT("texture-editor.import-images.progress-bar.title", "Importing Images"));
    progressBar.MakeDialog();

    TStrongObjectPtr<UTextureFactory> TextureFactory(NewObject<UTextureFactory>());
    TArray<TStrongObjectPtr<UTexture2D>> importedTextures;
    importedTextures.Reserve(filenames.Num());
    for (const FString& filename : filenames)
    {
        progressBar.EnterProgressFrame();

        UObject* importedObject = UFactory::StaticImportObject(UTexture2D::StaticClass(), GetTransientPackage(), NAME_None, EObjectFlags::RF_NoFlags, *filename, nullptr, TextureFactory.Get());
        UTexture2D* importedTexture = Cast<UTexture2D>(importedObject);
        if (!importedTexture)
            continue;

        importedTextures.Emplace(importedTexture);
    }

    TArray<UTexture2D*> textures;

    for (int i = 0; i < importedTextures.Num(); i++)
    {
        textures.Add(importedTextures[i].Get());
    }

    SOdysseyImportTexturesDialog::FInputParams inputParams;
    inputParams.Title = LOCTEXT("import-textures-dialog.title", "Import Images" );
    inputParams.CanvasWidth = texture->GetSurfaceWidth();
    inputParams.CanvasHeight = texture->GetSurfaceHeight();
    inputParams.Textures = textures;

    if(!SOdysseyImportTexturesDialog::Open(inputParams))
        return;

    /*FOdysseyPainterEditorTextureImport import;
    import.ImportTextures(currentTexture, texturesToImport);*/
}

void
FOdysseyPainterEditorGlobalTextureShortcuts::Action_ImportTextures()
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

    SOdysseyImportTexturesDialog::FInputParams inputParams;
    inputParams.Title = LOCTEXT("import-textures-dialog.title", "Import Textures" );
    inputParams.CanvasWidth = currentTexture->GetSurfaceWidth();
    inputParams.CanvasHeight = currentTexture->GetSurfaceHeight();
    inputParams.Textures = texturesToImport;

    if(!SOdysseyImportTexturesDialog::Open(inputParams))
        return;

    /*FOdysseyPainterEditorTextureImport import;
    import.ImportTextures(currentTexture, texturesToImport);*/
}
