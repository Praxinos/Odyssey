// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyTextureDummy/OdysseyTextureDummy_ContentBrowserExtensions.h"

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

#include "IOdysseyPainterEditorModule.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureDummy_ContentBrowserExtensions"

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
    //FAssetToolsModule& assetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>( "AssetTools" );
    //FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );

    for( auto textureIt = iTextures.CreateConstIterator(); textureIt; ++textureIt )
    {
        UTexture2D* texture = *textureIt;
        IOdysseyPainterEditorModule* odysseyPainterEditorModule = &FModuleManager::GetModuleChecked<IOdysseyPainterEditorModule>( "OdysseyPainterEditor" );
        odysseyPainterEditorModule->CreateOdysseyPainterEditor( EToolkitMode::Standalone, NULL, texture );
    }
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyPainterContentBrowserExtensions_Impl
class FOdysseyPainterContentBrowserExtensions_Impl
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
FOdysseyPainterContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor( TSharedPtr<FContentBrowserSelectedAssetExtensionBase> iSelectedAssetFunctor )
{
    iSelectedAssetFunctor->Execute();
}

//static
void
FOdysseyPainterContentBrowserExtensions_Impl::PopulateTextureActionsMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    TSharedPtr<FEditTextureExtension> editTextureFunctor = MakeShareable( new FEditTextureExtension() );
    editTextureFunctor->mSelectedAssets = iSelectedAssets;

    FUIAction action_EditTexture(
        FExecuteAction::CreateStatic( &FOdysseyPainterContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor, StaticCastSharedPtr<FContentBrowserSelectedAssetExtensionBase>( editTextureFunctor ) ) );

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "CB_Extension_Texture_OpenPaintEditor", "Edit with ILIAD" )
        , LOCTEXT( "CB_Extension_Texture_OpenPaintEditor_Tooltip", "Open ILIAD paint editor for the selected Texture" )
        , FSlateIcon( "OdysseyStyle", "PainterEditor.OpenPaintEditor16" )
        , action_EditTexture
        , NAME_None
        , EUserInterfaceActionType::Button );
}

//static
TSharedRef<FExtender>
FOdysseyPainterContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu( const TArray<FAssetData>& iSelectedAssets )
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
            , FMenuExtensionDelegate::CreateStatic( &FOdysseyPainterContentBrowserExtensions_Impl::PopulateTextureActionsMenu, iSelectedAssets ) );
    }

    return extender;
}

//static
TArray<FContentBrowserMenuExtender_SelectedAssets>&
FOdysseyPainterContentBrowserExtensions_Impl::GetExtenderDelegates()
{
    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );

    return contentBrowserModule.GetAllAssetViewContextMenuExtenders();
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyPainterContentBrowserExtensions
//static
void
FOdysseyPainterContentBrowserExtensions::InstallHooks()
{
    sgContentBrowserExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic( &FOdysseyPainterContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu );

    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = FOdysseyPainterContentBrowserExtensions_Impl::GetExtenderDelegates();
    cbMenuExtenderDelegates.Add( sgContentBrowserExtenderDelegate );
    sgContentBrowserExtenderDelegateHandle = cbMenuExtenderDelegates.Last().GetHandle();
}

//static
void
FOdysseyPainterContentBrowserExtensions::RemoveHooks()
{
    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = FOdysseyPainterContentBrowserExtensions_Impl::GetExtenderDelegates();
    cbMenuExtenderDelegates.RemoveAll( []( const FContentBrowserMenuExtender_SelectedAssets& Delegate ) { return Delegate.GetHandle() == sgContentBrowserExtenderDelegateHandle; } );
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
