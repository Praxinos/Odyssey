// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorAnimationExportShortcuts.h"

#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "Engine/Texture2D.h"
#include "Factories/TextureFactory.h"
#include "IContentBrowserSingleton.h"
#include "PaperFlipbook.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyPainterEditorAnimationFunctionLibrary.h"
#include "OdysseyPainterEditorAnimationImport.h"
#include "OdysseyPainterEditorAnimationSource.h"
#include "SOdysseyAnimationExportImageSequenceDialog.h"
#include "SOdysseyAnimationExportTextureSequenceDialog.h"
#include "SOdysseyImportTexturesDialog.h"


#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorAnimationExportShortcuts::FOdysseyPainterEditorAnimationExportShortcuts(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
}

void
FOdysseyPainterEditorAnimationExportShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    FOdysseyEditorShortcuts::MapActionsToCommandList(iCommandList);

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ExportAsFlipbook,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorAnimationExportShortcuts::Action_ExportAsFlipbook)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ExportImageSequence,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorAnimationExportShortcuts::Action_ExportImageSequence)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ExportTextureSequence,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorAnimationExportShortcuts::Action_ExportTextureSequence)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ImportImageSequence,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorAnimationExportShortcuts::Action_ImportImageSequence)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ImportTextureSequence,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorAnimationExportShortcuts::Action_ImportTextureSequence)
    );
}

void
FOdysseyPainterEditorAnimationExportShortcuts::Action_ExportAsFlipbook()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorAnimationSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorAnimationSource> animationSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(source);
    UOdysseyAnimation* animation = animationSource->GetAnimation();

    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "export-as-flipbook.save-asset-dialog.title", "Export As Flipbook" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath(animation->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = animation->GetName() + TEXT("_Flipbook");
    saveAssetDialogConfig.AssetClassNames.Add( UPaperFlipbook::StaticClass()->GetClassPathName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );

    if ( saveObjectPath == "" )
        return;

    FString assetPath = FPaths::GetPath(saveObjectPath) + "/";
    FString flipbookAssetName = FPaths::GetBaseFilename(saveObjectPath);


    UOdysseyPainterEditorAnimationFunctionLibrary::ExportAsFlipbook(animation, animation->GetFrameRange(), flipbookAssetName, assetPath);
}

void
FOdysseyPainterEditorAnimationExportShortcuts::Action_ExportTextureSequence()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorAnimationSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorAnimationSource> animationSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(source);

    UOdysseyAnimation* animation = animationSource->GetAnimation();

    SOdysseyAnimationExportTextureSequenceDialog::Open(animation);
}

void
FOdysseyPainterEditorAnimationExportShortcuts::Action_ExportImageSequence()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorAnimationSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorAnimationSource> animationSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(source);

    UOdysseyAnimation* animation = animationSource->GetAnimation();

    SOdysseyAnimationExportImageSequenceDialog::Open(animation);
}

void
FOdysseyPainterEditorAnimationExportShortcuts::Action_ImportImageSequence()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorAnimationSource::StaticId())
        return;

    TSharedPtr<FOdysseyPainterEditorAnimationSource> animationSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(source);

    UOdysseyAnimation* animation = animationSource->GetAnimation();

    IDesktopPlatform* desktopPlatformHandle = FDesktopPlatformModule::Get();
    TArray< FString > filenames;
    bool dialogValidated = desktopPlatformHandle->OpenFileDialog(
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr)
        , LOCTEXT("animation.import-images.dialog.title", "Select Images to import").ToString()
        , FString() //Allows the dialog to open in the last folder the dialog opened
        , animation->GetName()
        , TEXT("Image Files (*.png;*.bmp;*.tga;*.jpg;*.jpeg)|*.png;*.bmp;*.tga;*.jpg;*.jpeg")
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
    importParameters.Init(filenames, animation->GetWidth(), animation->GetHeight());
    if(!SOdysseyImportTexturesDialog::Open(LOCTEXT("animation-editor.import-textures-dialog.title", "Import Images" ), importParameters))
        return;

    FOdysseyPainterEditorAnimationImportResult importResult = FOdysseyPainterEditorAnimationImport::ImportTextureSequence(animation, importParameters);

    if (!importResult.mImportedLayers.IsEmpty())
    {
        UOdysseyLayerStack* layerStack = animation->GetLayerStack();
        if (layerStack)
            layerStack->SetCurrentLayer(importResult.mImportedLayers[0]);
    }
}

void
FOdysseyPainterEditorAnimationExportShortcuts::Action_ImportTextureSequence()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorAnimationSource::StaticId())
        return;

    UOdysseyLayerStack* layerStack = source->GetLayerStack();
    if ( !layerStack )
        return;

    TSharedPtr<FOdysseyPainterEditorAnimationSource> animationSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(source);
    UOdysseyAnimation* animation = animationSource->GetAnimation();

    FOpenAssetDialogConfig openAssetDialogConfig;
    openAssetDialogConfig.DialogTitleOverride = LOCTEXT( "animation-editor.import-texture-sequence.open-asset-dialog.title", "Import Texture Sequence" );
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(animation->GetPathName() );
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
    importParameters.Init(texturesToImport, animation->GetWidth(), animation->GetHeight());
    if(!SOdysseyImportTexturesDialog::Open(LOCTEXT("animation-editor.import-texture-sequence-dialog.title", "Import Texture Sequence" ), importParameters))
        return;

    FOdysseyPainterEditorAnimationImportResult importResult = FOdysseyPainterEditorAnimationImport::ImportTextureSequence(animation, importParameters);

    if (!importResult.mImportedLayers.IsEmpty())
        layerStack->SetCurrentLayer(importResult.mImportedLayers[0]);
}

#undef LOCTEXT_NAMESPACE
