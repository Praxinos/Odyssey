// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureContentBrowserExtensions.h"

#include "OdysseyTextureEditorModule.h"
#include "AssetRegistry/AssetData.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "Engine/Texture2D.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Interfaces/ITextureEditorModule.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "IDesktopPlatform.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "Misc/MessageDialog.h"
#include "OdysseyScopedTextureSettings.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyPixelFormat.h"
#include "Textures/SlateIcon.h"
#include "OdysseyPainterEditorModule.h"
#include "ULISLoaderModule.h"
#include <ULIS>

#define LOCTEXT_NAMESPACE "TextureEditor"

//////////////////////////////////////////////////////////////////////////

static FContentBrowserMenuExtender_SelectedAssets sgContentBrowserExtenderDelegate;
static FDelegateHandle sgContentBrowserExtenderDelegateHandle;

//---

TArray<UTexture2D*>
GetOnlyTextureAssets(TArray<FAssetData> iSelectedAssets)
{
    TArray<UTexture2D*> textures;
    for( auto assetIt = iSelectedAssets.CreateConstIterator(); assetIt; ++assetIt )
    {
        const FAssetData& assetData = *assetIt;
        if( UTexture2D* texture = Cast<UTexture2D>( assetData.GetAsset() ) )
        {
            textures.Add( texture );
        }
    }
    return textures;
}

void
ExecuteExportTexture( TArray<UTexture2D*> iTextures )
{
    IDesktopPlatform* desktopPlatformHandle = FDesktopPlatformModule::Get();
    for( auto textureIt = iTextures.CreateConstIterator(); textureIt; ++textureIt )
    {
        TArray< FString > filenames;
        UTexture2D* currentTexture = *textureIt;
        bool saveSuccess = desktopPlatformHandle->SaveFileDialog(
            FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr)
            , LOCTEXT("content-browser-extension.export-texture.save-dialog.title", "Select Export Path & Name").ToString()
            , FPaths::ProjectDir()
            , currentTexture->GetName()
            , TEXT("PNG Image (.png)|*.png|BMP Image (.bmp)|*.bmp|TGA Image (.tga)|*.tga|JPG Image (.jpg)|*.jpg")
            , EFileDialogFlags::None
            , filenames
        );


        if( ( textureIt.GetIndex() != ( iTextures.Num() - 1 ) ) && ( !saveSuccess ) )
        {
            EAppReturnType::Type answer = FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("content-browser-extension.export-texture.cancel-dialog.message", "Continue the remaing files export ?"), LOCTEXT("content-browser-extension.export-texture.cancel-dialog.title", "Save cancelled"));
            if( answer == EAppReturnType::Yes )
            {
                continue;
            }
            else
            {
                break;
            }
        }

        if( filenames.Num() > 0 )
        {
            FString path( FPaths::ConvertRelativePathToFull( filenames[0] ) );
            std::string str = std::string( TCHAR_TO_UTF8( *path ) );
            std::string extension = std::string( TCHAR_TO_UTF8( *( FPaths::GetExtension( path, false ) ) ) );
            ::ULIS::eFileFormat exportImageFormat = ::ULIS::FileFormat_png;
            bool extensionFound = false;
            for( int i = 0; i <= ::ULIS::FileFormat_hdr; ++i )
            {
                if( extension == ::ULIS::kwImageFormat[i] )
                {
                    exportImageFormat = static_cast< ::ULIS::eFileFormat >( i );
                    extensionFound = true;
                    break;
                }
            }

            if( !extensionFound )
            {
                FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("content-browser-extension.export-texture.invalid-extension-dialog.message", "The file extension or the file format is not supported"), LOCTEXT("content-browser-extension.export-texture.invalid-extension-dialog.title", "Invalid extension"));
                continue;
            }

            FTexturePlatformData* platformData = currentTexture->GetPlatformData();
            ::ULIS::FBlock* odysseyBlockToSave = new ::ULIS::FBlock( platformData->SizeX, platformData->SizeY, ULISFormatForTextureSourceFormat( currentTexture->Source.GetFormat() ) );
            FOdysseyScopedTextureSettings settingsGuard = FOdysseyScopedTextureSettings::MakeUncompressedNoMipMaps( currentTexture );
            CopyUTextureSourceDataIntoBlock( odysseyBlockToSave, currentTexture );
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( odysseyBlockToSave->Format() );

            bool canSaveDirectly = false;
            ::ULIS::FContext::SaveBlockToDiskMetrics(*odysseyBlockToSave, exportImageFormat, &canSaveDirectly);
            if (canSaveDirectly)
            {
                ctx.SaveBlockToDisk(
                    *odysseyBlockToSave
                    , str
                    , exportImageFormat
                    , 100
                );

                ctx.Finish();
            }
            else
            {
                ::ULIS::eFormat format = odysseyBlockToSave->Model() == ::ULIS::ColorModel_GREY ? ::ULIS::Format_GA8 : ::ULIS::Format_RGBA8;
                if (exportImageFormat == ::ULIS::FileFormat_hdr)
                {
                    format = ::ULIS::Format_RGBAF;
                }

                ::ULIS::FBlock blockProxy(odysseyBlockToSave->Width(), odysseyBlockToSave->Height(), format);

                ::ULIS::FEvent eventConvert;
                ctx.ConvertFormat(
                    *odysseyBlockToSave
                    , blockProxy
                    , ULIS::FRectI::Auto
                    , ULIS::FVec2I(0)
                    , ULIS::FSchedulePolicy::CacheEfficient
                    , 0
                    , nullptr
                    , &eventConvert
                );

                ctx.SaveBlockToDisk(
                    blockProxy
                    , str
                    , exportImageFormat
                    , 100
                );

                ctx.Finish();
            }

            delete odysseyBlockToSave;
        }
    }
}

void
ExecuteEditWithOdysseyTextureEditor(TArray<UTexture2D*> iTextures)
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    for (UTexture2D* texture : iTextures)
    {
        if (!texture)
            continue;

        if (AssetEditorSubsystem->FindEditorForAsset(texture, true))
            continue;

        FOdysseyPainterEditorModule* painterEditorModule = &FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
        painterEditorModule->OpenStandaloneEditorForAsset(texture);
    }
}

void
ExecuteEditWithDefaultTextureEditor(TArray<UTexture2D*> iTextures)
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    for (UTexture2D* texture : iTextures)
    {
        if (!texture)
            continue;

        if (AssetEditorSubsystem->FindEditorForAsset(texture, true))
            continue;

        ITextureEditorModule* TextureEditorModule = &FModuleManager::LoadModuleChecked<ITextureEditorModule>("TextureEditor");
        TextureEditorModule->CreateTextureEditor(EToolkitMode::Standalone, nullptr, texture);
    }
}

void
BuildEditWithSubMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    TArray<UTexture2D*> textures = GetOnlyTextureAssets(iSelectedAssets);
    if (textures.IsEmpty())
        return;

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "content-browser-extension.texture-action-menu.edit-with.odyssey-texture-editor.name", "Odyssey Texture Editor" )
        , LOCTEXT( "content-browser-extension.texture-action-menu.edit-with.odyssey-texture-editor.tooltip", "Edit Selected Textures with Odyssey Texture Editor" )
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&ExecuteEditWithOdysseyTextureEditor, textures))
        , NAME_None
        , EUserInterfaceActionType::Button );

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "content-browser-extension.texture-action-menu.edit-with.default-texture-editor.name", "Default Texture Editor" )
        , LOCTEXT( "content-browser-extension.texture-action-menu.edit-with.default-texture-editor.tooltip", "Edit Selected Textures with Default Texture Editor" )
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&ExecuteEditWithDefaultTextureEditor, textures))
        , NAME_None
        , EUserInterfaceActionType::Button );
}

void
BuildTextureActionsSection( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    TArray<UTexture2D*> textures = GetOnlyTextureAssets(iSelectedAssets);
    if (textures.IsEmpty())
        return;

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "content-browser-extension.texture-action-menu.odyssey.export-texture.name", "Export Texture" )
        , LOCTEXT( "content-browser-extension.texture-action-menu.odyssey.export-texture.tooltip", "Export Texture with Odyssey" )
        , FSlateIcon( "OdysseyStyle", "OdysseyTexture.ExportTexture_16" )
        , FUIAction(FExecuteAction::CreateStatic(&ExecuteExportTexture, textures))
        , NAME_None
        , EUserInterfaceActionType::Button );
}

void
BuildCommonSection( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    ioMenuBuilder.AddSubMenu(
          LOCTEXT( "content-browser-extension.texture-action-menu.edit-with-submenu.name", "Edit With" )
        , LOCTEXT( "content-browser-extension.texture-action-menu.edit-with-submenu.tooltip", "Edit selected textures using the editor of your choice" )
        , FNewMenuDelegate::CreateStatic( &BuildEditWithSubMenu, iSelectedAssets )
        , false
        , FSlateIcon(FAppStyle::GetAppStyleSetName(), TEXT("Icons.Edit"))
    );
}

TSharedRef<FExtender>
OnExtendContentBrowserAssetSelectionMenu( const TArray<FAssetData>& iSelectedAssets )
{
    TSharedRef<FExtender> extender( new FExtender() );

    bool isAnyTextures = false;
    for( auto assetIt = iSelectedAssets.CreateConstIterator(); assetIt; ++assetIt )
    {
        const FAssetData& asset = *assetIt;
        isAnyTextures = isAnyTextures || ( asset.GetClass() == UTexture2D::StaticClass() );
    }

    if( isAnyTextures )
    {
        // Add the edit texture action to the menu
        extender->AddMenuExtension(
              "GetAssetActions"
            , EExtensionHook::After
            , nullptr
            , FMenuExtensionDelegate::CreateStatic( &BuildTextureActionsSection, iSelectedAssets ) );

        extender->AddMenuExtension(
              "CommonAssetActions"
            , EExtensionHook::First
            , nullptr
            , FMenuExtensionDelegate::CreateStatic( &BuildCommonSection, iSelectedAssets ) );
    }

    return extender;
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyTextureContentBrowserExtensions

TArray<FContentBrowserMenuExtender_SelectedAssets>&
GetExtenderDelegates()
{
    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );
    return contentBrowserModule.GetAllAssetViewContextMenuExtenders();
}

//static
void
FOdysseyTextureContentBrowserExtensions::InstallHooks()
{
    sgContentBrowserExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic( &OnExtendContentBrowserAssetSelectionMenu );

    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = GetExtenderDelegates();
    cbMenuExtenderDelegates.Add( sgContentBrowserExtenderDelegate );
    sgContentBrowserExtenderDelegateHandle = cbMenuExtenderDelegates.Last().GetHandle();
}

//static
void
FOdysseyTextureContentBrowserExtensions::RemoveHooks()
{
    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = GetExtenderDelegates();
    cbMenuExtenderDelegates.RemoveAll( []( const FContentBrowserMenuExtender_SelectedAssets& Delegate ) { return Delegate.GetHandle() == sgContentBrowserExtenderDelegateHandle; } );
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
