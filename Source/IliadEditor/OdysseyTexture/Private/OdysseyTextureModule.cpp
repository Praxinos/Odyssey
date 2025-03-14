// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyTextureModule.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

/*-----------------------------------------------------------------------------
   FOdysseyTextureModule
-----------------------------------------------------------------------------*/

/*void
FOdysseyTextureModule::CreateOdysseyTexture( TArray<UTexture2D*> iTextures )
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem< UAssetEditorSubsystem >();
    bool warningDisplayed = false;
    for( UTexture2D* texture : iTextures )
    {
        //PATCH: To avoid opening Odyssey when another editor for this asset is opened
        // To make it right, we should use AssetEditorSubsystem->OpenEditorForAsset, but for now it would call the default editor instead of Odyssey
        if (AssetEditorSubsystem->FindEditorForAsset(texture, true) != nullptr)
        {
            if (!warningDisplayed)
            {
                FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("content-browser-extension.texture-already-opened-dialog.message", "The texture is already opened in an other editor. Please close the editor before opening the texture with Odyssey."), LOCTEXT("content-browser-extension.edit-texture.texture-already-opened-dialog.title", "Texture Already Opened"));
                warningDisplayed = true;
            }
            continue;
        }

        TSharedPtr<FOdysseyPainterEditor> editor = MakeShared<FOdysseyPainterEditor>(
            TEXT("OdysseyTexture"),
            LOCTEXT("main-menu.category", "Odyssey Texture2D Editor"),
            texture,
            "OdysseyTexture_Layout"
        );

        TSharedRef<FOdysseyTextureExtension> textureExtension = MakeShared<FOdysseyTextureExtension>(editor.Get());
        editor->AddExtension(textureExtension);

        TSharedPtr<FOdysseyTextureToolkit> toolkit = MakeShared<FOdysseyTextureToolkit>();
        toolkit->Initialize(texture, editor);

        TSharedPtr<FOdysseyTextureSource> source = MakeShared<FOdysseyTextureSource>(texture);
        editor->SetSource(source);
    }
}-*/

void
FOdysseyTextureModule::StartupModule()
{
}

void
FOdysseyTextureModule::ShutdownModule()
{
}

IMPLEMENT_MODULE( FOdysseyTextureModule, OdysseyTexture );

#undef LOCTEXT_NAMESPACE
