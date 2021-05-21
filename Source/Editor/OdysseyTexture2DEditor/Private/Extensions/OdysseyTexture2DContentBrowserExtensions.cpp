// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTexture2DContentBrowserExtensions.h"

#include "AssetData.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "EditorStyleSet.h"
#include "Engine/Texture2D.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "IDesktopPlatform.h"
#include "IOdysseyTexture2DEditorModule.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "OdysseyBlock.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "Textures/SlateIcon.h"
#include "ULISLoaderModule.h"
#include <ULIS3>

#define LOCTEXT_NAMESPACE "OdysseyTexture2DContentBrowserExtensions"

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

    virtual void ActionTextures( TArray< UTexture2D* >& iTextures ) = 0
    {}

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
	    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem< UAssetEditorSubsystem >();
        bool warningDisplayed = false;
        for( auto textureIt = iTextures.CreateConstIterator(); textureIt; ++textureIt )
        {
            UTexture2D* texture = *textureIt;

		    //PATCH: To avoid opening ILIAD when another editor for this asset is opened
		    // To make it right, we should use AssetEditorSubsystem->OpenEditorForAsset, but for now it would call the default editor instead of ILIAD
            if (AssetEditorSubsystem->FindEditorForAsset(texture, true) != nullptr)
            {
                if (!warningDisplayed)
                {
                    FText Title = LOCTEXT("TitleTextureAlreadyOpened", "Texture Already Opened");
                    FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("MessageTextureAlreadyOpened", "The texture is already opened in an other editor. Please close the editor before opening the texture with ILIAD."), &Title);
                    warningDisplayed = true;
                }
			    continue;
		    }
            IOdysseyTexture2DEditorModule* odysseyTexture2DEditorModule = &FModuleManager::GetModuleChecked< IOdysseyTexture2DEditorModule >( "OdysseyTexture2DEditor" );
            odysseyTexture2DEditorModule->CreateOdysseyTexture2DEditor( texture );
        }
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
            UTexture2D* texture = *textureIt;
            bool saveSuccess = desktopPlatformHandle->SaveFileDialog(
                  FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr)
                , LOCTEXT("TitleExportTexture", "Select Export Path & Name").ToString()
                , FPaths::ProjectDir()
                , texture->GetName()
                , TEXT("PNG Image (.png)|*.png|BMP Image (.bmp)|*.bmp|TGA Image (.tga)|*.tga|JPG Image (.jpg)|*.jpg|HDR Image (.HDR)|*.hdr")
                , EFileDialogFlags::None
                , filenames
            );


            if( ( textureIt.GetIndex() != ( iTextures.Num() - 1 ) ) && ( !saveSuccess ) )
            {
                FText Title = LOCTEXT("TitleSaveCancel", "Save cancelled");
                EAppReturnType::Type answer = FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("MessageSaveCancel", "Continue the remaing files export ?"), &Title);
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
                FOdysseyBlock* odysseyBlockToSave = NewOdysseyBlockFromUTextureData( texture, ULISFormatForUE4TextureSourceFormat( texture->Source.GetFormat() ) );
                ::ul3::FBlock* ulisBlockToSave = odysseyBlockToSave->GetBlock();
                IULISLoaderModule& hULIS = IULISLoaderModule::Get();
                FString path( FPaths::ConvertRelativePathToFull( filenames[0] ) );
                std::string str = std::string( TCHAR_TO_UTF8( *path ) );
                std::string extension = std::string( TCHAR_TO_UTF8( *( FPaths::GetExtension( path, false ) ) ) );
                ::ul3::eImageFormat exportImageFormat = ::ul3::eImageFormat::IM_PNG;
                bool extensionFound = false;
                for( int i = 0; i <= ::ul3::eImageFormat::IM_HDR; ++i )
                {
                    if( extension == ::ul3::kwImageFormat[i] )
                    {
                        exportImageFormat = static_cast< ::ul3::eImageFormat >( i );
                        extensionFound = true;
                        break;
                    }
                }

                if( !extensionFound )
                {
                    FText Title = LOCTEXT("TitleExtensionNotFound", "Invalid extension");
                    FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("MessageExtensionNotFound", "The file extension or the file format is not supported"), &Title);
                    continue;
                }

                ::ul3::SaveToFile(
                      hULIS.ThreadPool()
                    , true
                    , 0
                    , hULIS.HostDeviceInfo()
                    , false
                    , ulisBlockToSave
                    , str
                    , exportImageFormat
                    , 100
                );

                delete odysseyBlockToSave;
            }
        }
    }

};

//////////////////////////////////////////////////////////////////////////
// FOdysseyTexture2DContentBrowserExtensions_Impl
class FOdysseyTexture2DContentBrowserExtensions_Impl
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
FOdysseyTexture2DContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor( TSharedPtr<FContentBrowserSelectedAssetExtensionBase> iSelectedAssetFunctor )
{
    iSelectedAssetFunctor->Execute();
}

//static
void
FOdysseyTexture2DContentBrowserExtensions_Impl::PopulateTextureActionsMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    ioMenuBuilder.AddSubMenu(
          LOCTEXT( "CB_Extension_Texture_IliadActions", "ILIAD Actions" )
        , LOCTEXT( "CB_Extension_Texture_IliadActions_ToolTip", "All actions related to ILIAD" )
        , FNewMenuDelegate::CreateStatic( &FOdysseyTexture2DContentBrowserExtensions_Impl::PopulateTextureActionsSubMenu, iSelectedAssets )
        , false
        , FSlateIcon( "OdysseyStyle", "OdysseyTexture.Iliad16" )
    );
}

//static
void
FOdysseyTexture2DContentBrowserExtensions_Impl::PopulateTextureActionsSubMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    TSharedPtr<FEditTextureExtension> editTextureFunctor = MakeShared< FEditTextureExtension >( iSelectedAssets );
    TSharedPtr<FExportTextureExtension> exportTextureFunctor = MakeShared< FExportTextureExtension >( iSelectedAssets );

    FUIAction action_EditTexture(
        FExecuteAction::CreateStatic( &FOdysseyTexture2DContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor, StaticCastSharedPtr<FContentBrowserSelectedAssetExtensionBase>( editTextureFunctor ) ) );
    FUIAction action_ExportTexture(
        FExecuteAction::CreateStatic( &FOdysseyTexture2DContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor, StaticCastSharedPtr<FContentBrowserSelectedAssetExtensionBase>( exportTextureFunctor ) ) );

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "CB_Extension_Texture_OpenPaintEditor", "Edit Texture" )
        , LOCTEXT( "CB_Extension_Texture_OpenPaintEditor_Tooltip", "Open ILIAD paint editor for the selected Texture" )
        , FSlateIcon( "OdysseyStyle", "PainterEditor.OpenPaintEditor16" )
        , action_EditTexture
        , NAME_None
        , EUserInterfaceActionType::Button );

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "CB_Extension_Texture_Export", "Export Texture" )
        , LOCTEXT( "CB_Extension_Texture_Export_Tooltip", "Export Texture with ILIAD" )
        , FSlateIcon( "OdysseyStyle", "OdysseyTexture.ExportTexture_16" )
        , action_ExportTexture
        , NAME_None
        , EUserInterfaceActionType::Button );
}

//static
TSharedRef<FExtender>
FOdysseyTexture2DContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu( const TArray<FAssetData>& iSelectedAssets )
{
    TSharedRef<FExtender> extender( new FExtender() );

    bool isAnyTextures = false;
    for( auto assetIt = iSelectedAssets.CreateConstIterator(); assetIt; ++assetIt )
    {
        const FAssetData& asset = *assetIt;
        isAnyTextures = isAnyTextures || ( asset.AssetClass == UTexture2D::StaticClass()->GetFName() );
    }

    if( isAnyTextures )
    {
        // Add the edit texture action to the menu
        extender->AddMenuExtension(
              "GetAssetActions"
            , EExtensionHook::After
            , nullptr
            , FMenuExtensionDelegate::CreateStatic( &FOdysseyTexture2DContentBrowserExtensions_Impl::PopulateTextureActionsMenu, iSelectedAssets ) );
    }

    return extender;
}

//static
TArray<FContentBrowserMenuExtender_SelectedAssets>&
FOdysseyTexture2DContentBrowserExtensions_Impl::GetExtenderDelegates()
{
    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );

    return contentBrowserModule.GetAllAssetViewContextMenuExtenders();
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyTexture2DContentBrowserExtensions
//static
void
FOdysseyTexture2DContentBrowserExtensions::InstallHooks()
{
    sgContentBrowserExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic( &FOdysseyTexture2DContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu );

    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = FOdysseyTexture2DContentBrowserExtensions_Impl::GetExtenderDelegates();
    cbMenuExtenderDelegates.Add( sgContentBrowserExtenderDelegate );
    sgContentBrowserExtenderDelegateHandle = cbMenuExtenderDelegates.Last().GetHandle();
}

//static
void
FOdysseyTexture2DContentBrowserExtensions::RemoveHooks()
{
    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = FOdysseyTexture2DContentBrowserExtensions_Impl::GetExtenderDelegates();
    cbMenuExtenderDelegates.RemoveAll( []( const FContentBrowserMenuExtender_SelectedAssets& Delegate ) { return Delegate.GetHandle() == sgContentBrowserExtenderDelegateHandle; } );
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
