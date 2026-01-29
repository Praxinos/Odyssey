// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyTextureImportImagesDialog.h"
#include "SOdysseyImportTexturePositioning.h"
#include "Dialog/SCustomDialog.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

bool
SOdysseyTextureImportImagesDialog::Open(UTexture* oTexture, TArray< FString > iFilenames)
{
    TSharedPtr<SCustomDialog> customDialog;

    FText dialogTitle = LOCTEXT("import-images-dialog.title", "Import Images" );
    FText exportText = LOCTEXT("import-images-dialog.export", "Import" );
    FText cancelText = LOCTEXT("import-images-dialog.cancel", "Cancel");

    SWindow::FArguments windowArgs;
    windowArgs.MinWidth(500);
    windowArgs.MinHeight(500);

    TSharedRef<SOdysseyTextureImportImagesDialog> importWidget = SNew(SOdysseyTextureImportImagesDialog);
    customDialog = SNew( SCustomDialog )
        .Title( dialogTitle )
        .UseScrollBox(false)
        .WindowArguments(windowArgs)
        .ClientSize(FVector2D(500, 500)) //this line also activates Window Resizing
        .Buttons( { SCustomDialog::FButton( exportText ), SCustomDialog::FButton( cancelText ) } )
        .Content()
        [
            importWidget
        ];

    if (customDialog->ShowModal() != 0)
        return false;

    importWidget->Import();
    return true;
}

// Construction / Destruction
SOdysseyTextureImportImagesDialog::~SOdysseyTextureImportImagesDialog()
{

}

void
SOdysseyTextureImportImagesDialog::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SOdysseyImportTexturePositioning)
    ];
}

void
SOdysseyTextureImportImagesDialog::Import()
{

    /* FOdysseyPainterEditorTextureImport import;
    import.ImportImages(texture, filenames); */
}

#undef LOCTEXT_NAMESPACE
