// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookContentBrowserExtensions.h"

#include "AssetRegistry/AssetData.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "EditorStyleSet.h"
#include "PaperFlipbook.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "Textures/SlateIcon.h"
#include "PaperSprite.h"

#include "IOdysseyFlipbookEditorModule.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"

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
// FEditFlipbookExtension
struct FEditFlipbookExtension 
    : public FContentBrowserSelectedAssetExtensionBase
{
public:
    FEditFlipbookExtension();

public:
    virtual void Execute() override;

    void EditFlipbooks( TArray<UPaperFlipbook*>& Flipbooks );
};

//---

FEditFlipbookExtension::FEditFlipbookExtension()
    : FContentBrowserSelectedAssetExtensionBase()
{
}

void
FEditFlipbookExtension::Execute()
{
    TArray<UPaperFlipbook*> Flipbooks;
    for( auto assetIt = mSelectedAssets.CreateConstIterator(); assetIt; ++assetIt )
    {
        const FAssetData& assetData = *assetIt;
        if( UPaperFlipbook* Flipbook = Cast<UPaperFlipbook>( assetData.GetAsset() ) )
        {
            Flipbooks.Add( Flipbook );
        }
    }

    EditFlipbooks( Flipbooks );
}

void
FOdysseyFlipbookContentBrowserExtensions::EditFlipbooksWarning()
{
    FMessageDialog::Open(
        EAppMsgType::Ok,
        LOCTEXT("content-browser-extension.flipbook-already-opened.message", "The flipbook or one of its sprite or textures is already opened in an other editor. Please close the editor before opening the flipbook with ILIAD."),
        LOCTEXT("content-browser-extension.flipbook-already-opened.title", "Flipbook Already Opened"));
}

void
FEditFlipbookExtension::EditFlipbooks( TArray<UPaperFlipbook*>& iFlipbooks )
{
    IOdysseyFlipbookEditorModule* odysseyFlipbookEditorModule = &FModuleManager::GetModuleChecked<IOdysseyFlipbookEditorModule>( "OdysseyFlipbookEditor" );
    odysseyFlipbookEditorModule->CreateOdysseyFlipbookEditor( iFlipbooks );
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyFlipbookContentBrowserExtensions_Impl
class FOdysseyFlipbookContentBrowserExtensions_Impl
{
public:
    static void ExecuteSelectedContentFunctor( TSharedPtr<FContentBrowserSelectedAssetExtensionBase> iSelectedAssetFunctor );

    static void PopulateFlipbookActionsMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets );

    static TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu( const TArray<FAssetData>& iSelectedAssets );

    static TArray<FContentBrowserMenuExtender_SelectedAssets>& GetExtenderDelegates();
};

//---

//static
void
FOdysseyFlipbookContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor( TSharedPtr<FContentBrowserSelectedAssetExtensionBase> iSelectedAssetFunctor )
{
    iSelectedAssetFunctor->Execute();
}

//static
void
FOdysseyFlipbookContentBrowserExtensions_Impl::PopulateFlipbookActionsMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    TSharedPtr<FEditFlipbookExtension> editFlipbookFunctor = MakeShareable( new FEditFlipbookExtension() );
    editFlipbookFunctor->mSelectedAssets = iSelectedAssets;

    FUIAction action_EditFlipbook(
        FExecuteAction::CreateStatic( &FOdysseyFlipbookContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor, StaticCastSharedPtr<FContentBrowserSelectedAssetExtensionBase>( editFlipbookFunctor ) ) );

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "content-browser-extension.action-menu.edit-flipbook.name", "Edit Flipbook with ILIAD" )
        , LOCTEXT( "content-browser-extension.action-menu.edit-flipbook.tooltip", "Open ILIAD paint editor for the selected Flipbook" )
        , FSlateIcon( "OdysseyStyle", "PainterEditor.OpenPaintEditor16" )
        , action_EditFlipbook
        , NAME_None
        , EUserInterfaceActionType::Button );
}

//static
TSharedRef<FExtender>
FOdysseyFlipbookContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu( const TArray<FAssetData>& iSelectedAssets )
{
    TSharedRef<FExtender> extender( new FExtender() );

    bool isAnyFlipbooks = false;
    for( auto assetIt = iSelectedAssets.CreateConstIterator(); assetIt; ++assetIt )
    {
        const FAssetData& asset = *assetIt;
        isAnyFlipbooks = isAnyFlipbooks || ( asset.GetClass() == UPaperFlipbook::StaticClass() );
    }

    if( isAnyFlipbooks )
    {
        // Add the edit Flipbook action to the menu
        extender->AddMenuExtension(
              "GetAssetActions"
            , EExtensionHook::After
            , nullptr
            , FMenuExtensionDelegate::CreateStatic( &FOdysseyFlipbookContentBrowserExtensions_Impl::PopulateFlipbookActionsMenu, iSelectedAssets ) );
    }

    return extender;
}

//static
TArray<FContentBrowserMenuExtender_SelectedAssets>&
FOdysseyFlipbookContentBrowserExtensions_Impl::GetExtenderDelegates()
{
    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );

    return contentBrowserModule.GetAllAssetViewContextMenuExtenders();
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyFlipbookContentBrowserExtensions
//static
void
FOdysseyFlipbookContentBrowserExtensions::InstallHooks()
{
    sgContentBrowserExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic( &FOdysseyFlipbookContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu );

    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = FOdysseyFlipbookContentBrowserExtensions_Impl::GetExtenderDelegates();
    cbMenuExtenderDelegates.Add( sgContentBrowserExtenderDelegate );
    sgContentBrowserExtenderDelegateHandle = cbMenuExtenderDelegates.Last().GetHandle();
}

//static
void
FOdysseyFlipbookContentBrowserExtensions::RemoveHooks()
{
    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = FOdysseyFlipbookContentBrowserExtensions_Impl::GetExtenderDelegates();
    cbMenuExtenderDelegates.RemoveAll( []( const FContentBrowserMenuExtender_SelectedAssets& Delegate ) { return Delegate.GetHandle() == sgContentBrowserExtenderDelegateHandle; } );
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
