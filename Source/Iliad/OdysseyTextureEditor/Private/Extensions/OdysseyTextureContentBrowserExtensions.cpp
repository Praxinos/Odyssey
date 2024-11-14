// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureContentBrowserExtensions.h"

#include "AssetRegistry/AssetData.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "EditorStyleSet.h"
#include "Engine/Texture2D.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "IDesktopPlatform.h"
#include "IOdysseyTextureEditorModule.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "Misc/MessageDialog.h"
#include "OdysseyScopedTextureSettings.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyPixelFormat.h"
#include "Textures/SlateIcon.h"
#include "ULISLoaderModule.h"
#include <ULIS>

#define LOCTEXT_NAMESPACE "TextureEditor"

//////////////////////////////////////////////////////////////////////////

static FContentBrowserMenuExtender_SelectedAssets sgContentBrowserExtenderDelegate;
static FDelegateHandle sgContentBrowserExtenderDelegateHandle;

//////////////////////////////////////////////////////////////////////////
// FContentBrowserSelectedAssetExtensionBase
class FContentBrowserSelectedAssetExtensionBase
{
public:
    virtual ~FContentBrowserSelectedAssetExtensionBase()
    {}

    FContentBrowserSelectedAssetExtensionBase( const TArray< FAssetData >& iSelectedAssets )
        : mSelectedAssets( iSelectedAssets )
    {}

    void Execute()
    {
        TArray<UTexture2D*> textures;
        for( auto assetIt = mSelectedAssets.CreateConstIterator(); assetIt; ++assetIt )
        {
            const FAssetData& assetData = *assetIt;
            if( UTexture2D* texture = Cast<UTexture2D>( assetData.GetAsset() ) )
            {
                textures.Add( texture );
            }
        }

        ActionTextures( textures );
    }

    virtual void ActionTextures( TArray< UTexture2D* >& iTextures ) = 0;

protected:
    TArray< FAssetData > mSelectedAssets;
};

//////////////////////////////////////////////////////////////////////////
// FEditTextureExtension
class FEditTextureExtension
    : public FContentBrowserSelectedAssetExtensionBase
{
public:
    ~FEditTextureExtension() override
    {}

    FEditTextureExtension( const TArray< FAssetData >& iSelectedAssets )
        : FContentBrowserSelectedAssetExtensionBase( iSelectedAssets )
    {}

    void ActionTextures( TArray< UTexture2D* >& iTextures ) override
    {
        IOdysseyTextureEditorModule* OdysseyTextureEditorModule = &FModuleManager::GetModuleChecked< IOdysseyTextureEditorModule >( "OdysseyTextureEditor" );
        OdysseyTextureEditorModule->CreateOdysseyTextureEditor( iTextures );
    }
};

//////////////////////////////////////////////////////////////////////////
// FExportTextureExtension

class FExportTextureExtension
    : public FContentBrowserSelectedAssetExtensionBase
{
public:
    ~FExportTextureExtension() override
    {}

    FExportTextureExtension( const TArray< FAssetData >& iSelectedAssets )
        : FContentBrowserSelectedAssetExtensionBase( iSelectedAssets )
    {}

    void ActionTextures( TArray<UTexture2D*>& iTextures ) override
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

};

//////////////////////////////////////////////////////////////////////////
// FOdysseyTextureContentBrowserExtensions_Impl
class FOdysseyTextureContentBrowserExtensions_Impl
{
public:
    static void ExecuteSelectedContentFunctor( TSharedPtr<FContentBrowserSelectedAssetExtensionBase> iSelectedAssetFunctor );

    // we keep the iSelectedAssets type without ref and const, because CreateStatic discards qualifiers
    static void PopulateTextureActionsMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets );
    static void PopulateTextureActionsSubMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets );

    static TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu( const TArray<FAssetData>& iSelectedAssets );

    static TArray<FContentBrowserMenuExtender_SelectedAssets>& GetExtenderDelegates();
};

//---

//static
void
FOdysseyTextureContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor( TSharedPtr<FContentBrowserSelectedAssetExtensionBase> iSelectedAssetFunctor )
{
    iSelectedAssetFunctor->Execute();
}

//static
void
FOdysseyTextureContentBrowserExtensions_Impl::PopulateTextureActionsMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    ioMenuBuilder.AddSubMenu(
          LOCTEXT( "content-browser-extension.texture-action-menu.odyssey-submenu.name", "Odyssey Actions" )
        , LOCTEXT( "content-browser-extension.texture-action-menu.odyssey-submenu.tooltip", "All actions related to Odyssey" )
        , FNewMenuDelegate::CreateStatic( &FOdysseyTextureContentBrowserExtensions_Impl::PopulateTextureActionsSubMenu, iSelectedAssets )
        , false
        , FSlateIcon( "OdysseyStyle", "OdysseyLogo.Odyssey16" )
    );
}

//static
void
FOdysseyTextureContentBrowserExtensions_Impl::PopulateTextureActionsSubMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    TSharedPtr<FEditTextureExtension> editTextureFunctor = MakeShared< FEditTextureExtension >( iSelectedAssets );
    TSharedPtr<FExportTextureExtension> exportTextureFunctor = MakeShared< FExportTextureExtension >( iSelectedAssets );

    FUIAction action_EditTexture(
        FExecuteAction::CreateStatic( &FOdysseyTextureContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor, StaticCastSharedPtr<FContentBrowserSelectedAssetExtensionBase>( editTextureFunctor ) ) );
    FUIAction action_ExportTexture(
        FExecuteAction::CreateStatic( &FOdysseyTextureContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor, StaticCastSharedPtr<FContentBrowserSelectedAssetExtensionBase>( exportTextureFunctor ) ) );

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "content-browser-extension.texture-action-menu.odyssey.edit-texture.name", "Edit Texture" )
        , LOCTEXT( "content-browser-extension.texture-action-menu.odyssey.edit-texture.tooltip", "Open Odyssey paint editor for the selected Texture" )
        , FSlateIcon( "OdysseyStyle", "PainterEditor.OpenPaintEditor16" )
        , action_EditTexture
        , NAME_None
        , EUserInterfaceActionType::Button );

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "content-browser-extension.texture-action-menu.odyssey.export-texture.name", "Export Texture" )
        , LOCTEXT( "content-browser-extension.texture-action-menu.odyssey.export-texture.tooltip", "Export Texture with Odyssey" )
        , FSlateIcon( "OdysseyStyle", "OdysseyTexture.ExportTexture_16" )
        , action_ExportTexture
        , NAME_None
        , EUserInterfaceActionType::Button );
}

//static
TSharedRef<FExtender>
FOdysseyTextureContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu( const TArray<FAssetData>& iSelectedAssets )
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
            , FMenuExtensionDelegate::CreateStatic( &FOdysseyTextureContentBrowserExtensions_Impl::PopulateTextureActionsMenu, iSelectedAssets ) );
    }

    return extender;
}

//static
TArray<FContentBrowserMenuExtender_SelectedAssets>&
FOdysseyTextureContentBrowserExtensions_Impl::GetExtenderDelegates()
{
    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );

    return contentBrowserModule.GetAllAssetViewContextMenuExtenders();
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyTextureContentBrowserExtensions
//static
void
FOdysseyTextureContentBrowserExtensions::InstallHooks()
{
    sgContentBrowserExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic( &FOdysseyTextureContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu );

    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = FOdysseyTextureContentBrowserExtensions_Impl::GetExtenderDelegates();
    cbMenuExtenderDelegates.Add( sgContentBrowserExtenderDelegate );
    sgContentBrowserExtenderDelegateHandle = cbMenuExtenderDelegates.Last().GetHandle();
}

//static
void
FOdysseyTextureContentBrowserExtensions::RemoveHooks()
{
    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = FOdysseyTextureContentBrowserExtensions_Impl::GetExtenderDelegates();
    cbMenuExtenderDelegates.RemoveAll( []( const FContentBrowserMenuExtender_SelectedAssets& Delegate ) { return Delegate.GetHandle() == sgContentBrowserExtenderDelegateHandle; } );
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
