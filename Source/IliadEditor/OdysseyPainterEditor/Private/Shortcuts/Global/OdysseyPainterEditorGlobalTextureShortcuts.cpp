// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorGlobalTextureShortcuts.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorTextureCommands.h"
#include "OdysseyPainterEditorTextureSource.h"
#include "SOdysseyTextureExportAsImageDialog.h"
#include "SOdysseyTextureExportAsTextureDialog.h"

//Action_ImportTextures()
#include "ScopedTransaction.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "ULISLoaderModule.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyRasterBlockMutator.h"

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
