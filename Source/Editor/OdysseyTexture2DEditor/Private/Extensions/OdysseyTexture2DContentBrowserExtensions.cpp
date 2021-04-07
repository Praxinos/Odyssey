// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTexture2DContentBrowserExtensions.h"

#include "AssetData.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "EditorStyleSet.h"
#include "Engine/Texture2D.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "Textures/SlateIcon.h"
#include "IOdysseyTexture2DEditorModule.h"

#define LOCTEXT_NAMESPACE "OdysseyTexture2DContentBrowserExtensions"

//////////////////////////////////////////////////////////////////////////

static FContentBrowserMenuExtender_SelectedAssets sgContentBrowserExtenderDelegate;
static FDelegateHandle sgContentBrowserExtenderDelegateHandle;

//////////////////////////////////////////////////////////////////////////
// FContentBrowserSelectedAssetExtensionBase
struct FContentBrowserSelectedAssetExtensionBase
{
public:
    virtual ~FContentBrowserSelectedAssetExtensionBase();

    virtual void Execute();

public:
    TArray<struct FAssetData> mSelectedAssets;
};

//---

FContentBrowserSelectedAssetExtensionBase::~FContentBrowserSelectedAssetExtensionBase()
{
}

void 
FContentBrowserSelectedAssetExtensionBase::Execute()
{
}

//////////////////////////////////////////////////////////////////////////
// FEditTextureExtension
struct FEditTextureExtension 
    : public FContentBrowserSelectedAssetExtensionBase
{
public:
    FEditTextureExtension();

public:
    virtual void Execute() override;

    void EditTextures( TArray<UTexture2D*>& Textures );
};

//---

FEditTextureExtension::FEditTextureExtension()
    : FContentBrowserSelectedAssetExtensionBase()
{
}

void
FEditTextureExtension::Execute()
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

    EditTextures( textures );
}

void
FEditTextureExtension::EditTextures( TArray<UTexture2D*>& iTextures )
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
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
                FText Title = LOCTEXT("TitleDeletingCurrentLayer", "Texture Already Opened");
                FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("DeletingCurrentLayer", "The texture is already opened in an other editor. Please close the editor before opening the texture with ILIAD."), &Title);
                warningDisplayed = true;
            }
			continue;
		}
        IOdysseyTexture2DEditorModule* odysseyTexture2DEditorModule = &FModuleManager::GetModuleChecked<IOdysseyTexture2DEditorModule>( "OdysseyTexture2DEditor" );
        odysseyTexture2DEditorModule->CreateOdysseyTexture2DEditor( texture );
    }
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyTexture2DContentBrowserExtensions_Impl
class FOdysseyTexture2DContentBrowserExtensions_Impl
{
public:
    static void ExecuteSelectedContentFunctor( TSharedPtr<FContentBrowserSelectedAssetExtensionBase> iSelectedAssetFunctor );

    static void PopulateTextureActionsMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets );

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
    TSharedPtr<FEditTextureExtension> editTextureFunctor = MakeShareable( new FEditTextureExtension() );
    editTextureFunctor->mSelectedAssets = iSelectedAssets;

    FUIAction action_EditTexture(
        FExecuteAction::CreateStatic( &FOdysseyTexture2DContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor, StaticCastSharedPtr<FContentBrowserSelectedAssetExtensionBase>( editTextureFunctor ) ) );

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "CB_Extension_Texture_OpenPaintEditor", "Edit Texture with ILIAD" )
        , LOCTEXT( "CB_Extension_Texture_OpenPaintEditor_Tooltip", "Open ILIAD paint editor for the selected Texture" )
        , FSlateIcon( "OdysseyStyle", "PainterEditor.OpenPaintEditor16" )
        , action_EditTexture
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
