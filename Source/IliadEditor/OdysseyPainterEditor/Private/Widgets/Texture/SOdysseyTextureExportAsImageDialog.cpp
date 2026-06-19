// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyTextureExportAsImageDialog.h"

#include "Dialog/SCustomDialog.h"
#include "OdysseyTextureLayer.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyExportImage.h"
#include "SEnumCombo.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "DesktopPlatformModule.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"

#include "Misc/ScopedSlowTask.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

bool
SOdysseyTextureExportAsImageDialog::Open(UTexture2D* iTexture)
{
    TSharedPtr<SCustomDialog> customDialog;

    FText dialogTitle = LOCTEXT("export-image-dialog.title", "Export As Image" );
    FText exportText = LOCTEXT("export-image-dialog.export", "Export" );
    FText cancelText = LOCTEXT("export-image-dialog.cancel", "Cancel");


    TSharedRef<SOdysseyTextureExportAsImageDialog> exportImageWidget = SNew(SOdysseyTextureExportAsImageDialog, iTexture);
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
SOdysseyTextureExportAsImageDialog::Construct(const FArguments& InArgs, UTexture2D* iTexture)
{
    mExporter.mTexture = iTexture;

    ChildSlot
    [
        SNew(SGridPanel)
        + SGridPanel::Slot(0, 0)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("export-image-dialog.format.name", "Format"))
        ]
        + SGridPanel::Slot(0, 1)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("export-image-dialog.source.name", "Source"))
        ]
        + SGridPanel::Slot(1, 0)
        [
            SNew(SEnumComboBox, StaticEnum<EOdysseyExportImageFormat>())
            .ContentPadding(FMargin(0))
            .CurrentValue_Lambda([this](){ return (int32)mExporter.mFormat;})
            .OnEnumSelectionChanged_Lambda([this](int32 iValue, ESelectInfo::Type iSelectInfo){ mExporter.mFormat = (EOdysseyExportImageFormat)iValue;})
        ]
        + SGridPanel::Slot(1, 1)
        [
            SNew(SEnumComboBox, StaticEnum<EOdysseyTextureExportAsImageSource>())
            .ContentPadding(FMargin(0))
            .CurrentValue_Lambda([this](){ return (int32)mExporter.mSource;})
            .OnEnumSelectionChanged_Lambda([this](int32 iValue, ESelectInfo::Type iSelectInfo){ mExporter.mSource = (EOdysseyTextureExportAsImageSource)iValue;})
        ]
    ];
}

FString
SOdysseyTextureExportAsImageDialog::GetSaveFileDialogExtension()
{
    switch(mExporter.mFormat)
    {
        case EOdysseyExportImageFormat::PNG: return FString::Format(TEXT("{0} (.png)|*.png"), { LOCTEXT("export-image-dialog.format-extension.png", "PNG Image").ToString() } );
        case EOdysseyExportImageFormat::BMP: return FString::Format(TEXT("{0} (.bmp)|*.bmp"), { LOCTEXT("export-image-dialog.format-extension.bmp", "BMP Image").ToString() } );
        case EOdysseyExportImageFormat::TGA: return FString::Format(TEXT("{0} (.tga)|*.tga"), { LOCTEXT("export-image-dialog.format-extension.tga", "TGA Image").ToString() } );
        case EOdysseyExportImageFormat::Jpeg: return FString::Format(TEXT("{0} (.jpg)|*.jpg"), { LOCTEXT("export-image-dialog.format-extension.jpeg", "Jpeg Image").ToString() } );
    }

    return TEXT("");
}

FOdysseyTextureAsImageExporter::FOdysseyTextureAsImageExporter()
    : mTexture(nullptr)
{
}

FOdysseyTextureAsImageExporter::FOdysseyTextureAsImageExporter(UTexture2D* iTexture)
    : mTexture(iTexture)
{
}

TArray<FOdysseyTextureAsImageExporter::FSource>
FOdysseyTextureAsImageExporter::GetSources()
{
    UOdysseyTextureLayerStackUserData* textureUserData = Cast<UOdysseyTextureLayerStackUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
    if (!textureUserData)
        return {};

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(textureUserData->GetLayerStack());
    switch(mSource)
    {
        case EOdysseyTextureExportAsImageSource::AllLayers:
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
        case EOdysseyTextureExportAsImageSource::CurrentLayer:
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

        case EOdysseyTextureExportAsImageSource::SelectedLayers:
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
SOdysseyTextureExportAsImageDialog::Export()
{
    IDesktopPlatform* desktopPlatformHandle = FDesktopPlatformModule::Get();
    TArray< FString > filenames;
    bool saveSuccess = desktopPlatformHandle->SaveFileDialog(
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr)
        , LOCTEXT("texture.export-image.save-dialog.title", "Select Export Path & Name").ToString()
        , FPaths::ProjectDir()
        , mExporter.mTexture->GetName()
        , GetSaveFileDialogExtension()
        , EFileDialogFlags::None
        , filenames
    );

    if( !saveSuccess || filenames.Num() <= 0 )
        return;

    mExporter.Export(filenames[0]);
}

void
FOdysseyTextureAsImageExporter::Export(const FString& iFilename)
{
    TArray<FSource> sources = GetSources();

    FScopedSlowTask progressBar(sources.Num(), LOCTEXT("texture.export-image.progress-bar.title", "Exporting"));
    progressBar.MakeDialog();

    for (const FSource& source : sources)
    {
        progressBar.EnterProgressFrame();
        ExportSource(source, iFilename);
    }
}

void
FOdysseyTextureAsImageExporter::ExportSource(const FSource& iSource, const FString& iFilename)
{
    FString path( FPaths::ConvertRelativePathToFull( iFilename ) );
    FString folder = FPaths::GetPath(path);
    FString filename = FPaths::GetBaseFilename(path);
    if (!iSource.mFilename.IsEmpty())
        filename += TEXT("_") + iSource.mFilename;
    FString extension = FPaths::GetExtension(path, false);

    FString imageName = filename;
    imageName += TEXT(".") + extension;
    Odyssey::ExportAsImage(iSource.mTextureRenderingAbility, 0, mFormat, imageName, folder);
}

#undef LOCTEXT_NAMESPACE
