// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyTextureImportTexturesDialog.h"
#include "SOdysseyImportTexturePositioning.h"
#include "Dialog/SCustomDialog.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

bool
SOdysseyTextureImportTexturesDialog::Open(UTexture* oTexture, TArray< UTexture2D* > iTextures)
{
    TSharedPtr<SCustomDialog> customDialog;

    FText dialogTitle = LOCTEXT("import-textures-dialog.title", "Import Textures" );
    FText exportText = LOCTEXT("import-textures-dialog.export", "Import" );
    FText cancelText = LOCTEXT("import-textures-dialog.cancel", "Cancel");

    SWindow::FArguments windowArgs;
    windowArgs.MinWidth(500);
    windowArgs.MinHeight(500);

    TSharedRef<SOdysseyTextureImportTexturesDialog> importWidget = SNew(SOdysseyTextureImportTexturesDialog);
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
SOdysseyTextureImportTexturesDialog::~SOdysseyTextureImportTexturesDialog()
{

}

void
SOdysseyTextureImportTexturesDialog::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SOdysseyImportTexturePositioning)
    ];
}

void
SOdysseyTextureImportTexturesDialog::Import()
{
    /*FOdysseyPainterEditorTextureImport import;
    import.ImportTextures(currentTexture, texturesToImport);*/
}

#undef LOCTEXT_NAMESPACE
