// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyTextureExportAsTextureDialog.h"

#include "ContentBrowserModule.h"
#include "Dialog/SCustomDialog.h"
#include "IContentBrowserSingleton.h"
#include "OdysseyTextureLayer.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyExportImage.h"
#include "SEnumCombo.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SGridPanel.h"

#include "Misc/ScopedSlowTask.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

bool
SOdysseyTextureExportAsTextureDialog::Open(UTexture2D* iTexture)
{
    TSharedPtr<SCustomDialog> customDialog;

    FText dialogTitle = LOCTEXT("export-texture-dialog.title", "Export As Texture" );
    FText exportText = LOCTEXT("export-texture-dialog.export", "Export" );
    FText cancelText = LOCTEXT("export-texture-dialog.cancel", "Cancel");


    TSharedRef<SOdysseyTextureExportAsTextureDialog> exportImageWidget = SNew(SOdysseyTextureExportAsTextureDialog, iTexture);
    customDialog = SNew( SCustomDialog )
        .Title( dialogTitle )
        .Buttons( { SCustomDialog::FButton( exportText ), SCustomDialog::FButton( cancelText ) } )
        .Content()
        [
            exportImageWidget
        ];

    if (customDialog->ShowModal() != 0)
        return false;

    exportImageWidget->Export();
    return true;
}

void
SOdysseyTextureExportAsTextureDialog::Construct(const FArguments& InArgs, UTexture2D* iTexture)
{
    mExporter.mTexture = iTexture;

    const UEnum* sourceEnum = StaticEnum<EOdysseyTextureExportAsTextureSource>();

    ChildSlot
    [
        SNew(SGridPanel)
        + SGridPanel::Slot(0, 0)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("export-texture-dialog.source.name", "Source"))
        ]
        + SGridPanel::Slot(1, 0)
        [
            SNew(SEnumComboBox, sourceEnum)
            .ContentPadding(FMargin(0))
            .CurrentValue_Lambda([this](){ return (int32)mExporter.mSource;})
            .OnEnumSelectionChanged_Lambda([this](int32 iValue, ESelectInfo::Type iSelectInfo){ mExporter.mSource = (EOdysseyTextureExportAsTextureSource)iValue;})
        ]
    ];
}

FOdysseyTextureAsTextureExporter::FOdysseyTextureAsTextureExporter()
    : mTexture(nullptr)
{
}

FOdysseyTextureAsTextureExporter::FOdysseyTextureAsTextureExporter(UTexture2D* iTexture)
    : mTexture(iTexture)
{
}

TArray<FOdysseyTextureAsTextureExporter::FSource>
FOdysseyTextureAsTextureExporter::GetSources()
{
    UOdysseyTextureLayerStackUserData* textureUserData = Cast<UOdysseyTextureLayerStackUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
    if (!textureUserData)
        return {};

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(textureUserData->GetLayerStack());
    switch(mSource)
    {
        case EOdysseyTextureExportAsTextureSource::AllLayers:
        {
            TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
            TArray<FSource> sourceLayers;

            for (UOdysseyLayer* layer : layers)
            {
                if (layer->CanHaveChildren()) //do not export folders here
                    continue;

                UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layer);
                sourceLayers.Add(
                    {
                        textureLayer,
                        textureLayer->GetLayerName().ToString().Replace(TEXT(" "), TEXT("_")),
                    }
                );
            }

            return sourceLayers;
        }
        case EOdysseyTextureExportAsTextureSource::CurrentLayer:
        {
            UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layerStack->GetCurrentLayer());
            return
            {
                {
                    textureLayer,
                    textureLayer->GetLayerName().ToString().Replace(TEXT(" "), TEXT("_"))
                }
            };
        }

        case EOdysseyTextureExportAsTextureSource::SelectedLayers:
        {
            TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
            TArray<UOdysseyLayer*> selectedLayers;
            TArray<FSource> sourceLayers;

            selectedLayers.Reserve( layers.Num() );

            for (UOdysseyLayer* layer : layers)
            {
                if ( layer->IsSelected() )
                {
                    UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layer);

                    selectedLayers.Add( textureLayer );
                }
            }

            // add the current layer
            if ( selectedLayers.Find( layerStack->GetCurrentLayer() ) == INDEX_NONE )
            {
                UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->GetCurrentLayer());

                selectedLayers.Add( currentLayer );
            }

            for( UOdysseyLayer* selectedLayer : selectedLayers )
            {
                if ( !selectedLayer->CanHaveChildren() ) //do not export folders here
                {
                    UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(selectedLayer);

                    sourceLayers.Add(
                        {
                            textureLayer,
                            textureLayer->GetLayerName().ToString().Replace(TEXT(" "), TEXT("_"))
                        } );
                }
            }

            return sourceLayers;
        }
    }

    return {{nullptr, FString()}};
}

void
SOdysseyTextureExportAsTextureDialog::Export()
{
    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "export-layers-as-textures.save-asset-dialog.title", "Select Export Path & Name" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath(mExporter.mTexture->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = mExporter.mTexture->GetName();
    saveAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetClassPathName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );

    if ( saveObjectPath == "" )
        return;

    mExporter.Export(saveObjectPath);
}

void
FOdysseyTextureAsTextureExporter::Export(const FString& iPath)
{
    TArray<FSource> sources = GetSources();

    FScopedSlowTask progressBar(sources.Num(), LOCTEXT("texture.export-image.progress-bar.title", "Exporting"));
    progressBar.MakeDialog();

    for (const FSource& source : sources)
    {
        progressBar.EnterProgressFrame();
        ExportSource(source, iPath);
    }
}

void
FOdysseyTextureAsTextureExporter::ExportSource(const FSource& iSource, const FString& iPath)
{
    FString folder = FPaths::GetPath(iPath) + "/";
    FString filename = FPaths::GetBaseFilename(iPath);
    if (!iSource.mFilename.IsEmpty())
        filename += TEXT("_") + iSource.mFilename;

    Odyssey::ExportAsTexture(iSource.mTextureRenderingAbility, 0, filename, folder);
}

#undef LOCTEXT_NAMESPACE
