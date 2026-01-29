// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyTextureImportTexturesDialog.h"
#include "SOdysseyImportTexturePositioning.h"
#include "Dialog/SCustomDialog.h"
#include "Factories/TextureFactory.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

bool
SOdysseyTextureImportTexturesDialog::Open(UTexture* oTexture, TArray< UTexture2D* > iTextures)
{
    FText dialogTitle = LOCTEXT("import-textures-dialog.title", "Import Textures" );
    return Open(dialogTitle, oTexture, iTextures);
}

bool
SOdysseyTextureImportTexturesDialog::Open(UTexture* oTexture, TArray< FString > iFilenames)
{
    if ( iFilenames.Num() <= 0 || !oTexture )
        return false;

    FScopedSlowTask progressBar(iFilenames.Num(), LOCTEXT("texture-editor.import-images.progress-bar.title", "Importing Images"));
    progressBar.MakeDialog();

    TStrongObjectPtr<UTextureFactory> TextureFactory(NewObject<UTextureFactory>());
    TArray<TStrongObjectPtr<UTexture2D>> importedTextures;
    importedTextures.Reserve(iFilenames.Num());
    for (const FString& filename : iFilenames)
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

    FText dialogTitle = LOCTEXT("import-images-dialog.title", "Import Images" );

    return Open(dialogTitle, oTexture, textures);
}

bool
SOdysseyTextureImportTexturesDialog::Open(FText iTitle, UTexture* oTexture, TArray< UTexture2D* > iTextures)
{
    FText exportText = LOCTEXT("import-textures-dialog.export", "Import" );
    FText cancelText = LOCTEXT("import-textures-dialog.cancel", "Cancel");

    SWindow::FArguments windowArgs;
    windowArgs.MinWidth(500);
    windowArgs.MinHeight(500);

    TSharedRef<SOdysseyTextureImportTexturesDialog> importWidget = SNew(SOdysseyTextureImportTexturesDialog);


    TSharedPtr<SCustomDialog> customDialog = SNew( SCustomDialog )
        .Title( iTitle )
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
        .Data(mPositioningData)
    ];
}

void
SOdysseyTextureImportTexturesDialog::Import()
{
    /*FOdysseyPainterEditorTextureImport import;
    import.ImportTextures(currentTexture, texturesToImport);*/

    /* FOdysseyPainterEditorTextureImport import;
    import.ImportImages(texture, filenames); */
}

#undef LOCTEXT_NAMESPACE
