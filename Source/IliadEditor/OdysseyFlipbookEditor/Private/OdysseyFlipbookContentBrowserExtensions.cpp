// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyFlipbookContentBrowserExtensions.h"

#include "AssetToolsModule.h"
#include "ContentBrowserDelegates.h"
#include "ContentBrowserModule.h"
#include "PaperFlipbook.h"
#include "OdysseyFlipbookAssetTypeActions.h"
#include "OdysseyFlipbookEditorModule.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"

//////////////////////////////////////////////////////////////////////////

static FContentBrowserMenuExtender_SelectedAssets sgContentBrowserExtenderDelegate;
static FDelegateHandle sgContentBrowserExtenderDelegateHandle;

//---

TArray<UObject*>
GetOnlyFlipbookAssets(TArray<FAssetData> iSelectedAssets)
{
    TArray<UObject*> flipbooks;
    for( auto assetIt = iSelectedAssets.CreateConstIterator(); assetIt; ++assetIt )
    {
        const FAssetData& assetData = *assetIt;
        UObject* object = assetData.GetAsset();
        if( object->IsA<UPaperFlipbook>())
        {
            flipbooks.Add( object );
        }
    }
    return flipbooks;
}

void
ExecuteEditWithOdysseyFlipbookEditor(TArray<UObject*> iFlipbooks)
{
    FOdysseyFlipbookEditorModule& odysseyFlipbookEditorModule = FModuleManager::LoadModuleChecked<FOdysseyFlipbookEditorModule>(TEXT("OdysseyFlipbookEditor"));
    TSharedPtr<FOdysseyFlipbookAssetTypeActions> typeActions = odysseyFlipbookEditorModule.GetFlipbookTypeActions();
    typeActions->OpenOdysseyAssetEditor(iFlipbooks);
}

void
ExecuteEditWithDefaultFlipbookEditor(TArray<UObject*> iFlipbooks)
{
    FOdysseyFlipbookEditorModule& odysseyFlipbookEditorModule = FModuleManager::LoadModuleChecked<FOdysseyFlipbookEditorModule>(TEXT("OdysseyFlipbookEditor"));
    TSharedPtr<FOdysseyFlipbookAssetTypeActions> typeActions = odysseyFlipbookEditorModule.GetFlipbookTypeActions();
    typeActions->OpenPaper2DAssetEditor(iFlipbooks);
}

//---

void
BuildEditWithSubMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    TArray<UObject*> flipbooks = GetOnlyFlipbookAssets(iSelectedAssets);
    if (flipbooks.IsEmpty())
        return;

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "content-browser-extension.flipbook-action-menu.edit-with.odyssey-flipbook-editor.name", "Odyssey Flipbook Editor" )
        , LOCTEXT( "content-browser-extension.flipbook-action-menu.edit-with.odyssey-flipbook-editor.tooltip", "Edit Selected Flipbooks with Odyssey Flipbook Editor" )
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&ExecuteEditWithOdysseyFlipbookEditor, flipbooks))
        , NAME_None
        , EUserInterfaceActionType::Button );

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "content-browser-extension.flipbook-action-menu.edit-with.default-flipbook-editor.name", "Default Flipbook Editor" )
        , LOCTEXT( "content-browser-extension.flipbook-action-menu.edit-with.default-flipbook-editor.tooltip", "Edit Selected Flipbooks with Default Flipbook Editor" )
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&ExecuteEditWithDefaultFlipbookEditor, flipbooks))
        , NAME_None
        , EUserInterfaceActionType::Button );
}

void
BuildCommonSection( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    ioMenuBuilder.AddSubMenu(
          LOCTEXT( "content-browser-extension.flipbook-action-menu.edit-with-submenu.name", "Edit With" )
        , LOCTEXT( "content-browser-extension.flipbook-action-menu.edit-with-submenu.tooltip", "Edit selected flipbooks using the editor of your choice" )
        , FNewMenuDelegate::CreateStatic( &BuildEditWithSubMenu, iSelectedAssets )
        , false
        , FSlateIcon(FAppStyle::GetAppStyleSetName(), TEXT("Icons.Edit"))
    );
}

TSharedRef<FExtender>
OnExtendContentBrowserAssetSelectionMenu( const TArray<FAssetData>& iSelectedAssets )
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
        // Add the edit flipbook action to the menu

        extender->AddMenuExtension(
              "CommonAssetActions"
            , EExtensionHook::First
            , nullptr
            , FMenuExtensionDelegate::CreateStatic( &BuildCommonSection, iSelectedAssets ) );
    }

    return extender;
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyFlipbookContentBrowserExtensions

TArray<FContentBrowserMenuExtender_SelectedAssets>&
GetExtenderDelegates()
{
    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );
    return contentBrowserModule.GetAllAssetViewContextMenuExtenders();
}

//static
void
FOdysseyFlipbookContentBrowserExtensions::InstallHooks()
{
    sgContentBrowserExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic( &OnExtendContentBrowserAssetSelectionMenu );

    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = GetExtenderDelegates();
    cbMenuExtenderDelegates.Add( sgContentBrowserExtenderDelegate );
    sgContentBrowserExtenderDelegateHandle = cbMenuExtenderDelegates.Last().GetHandle();
}

//static
void
FOdysseyFlipbookContentBrowserExtensions::RemoveHooks()
{
    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = GetExtenderDelegates();
    cbMenuExtenderDelegates.RemoveAll( []( const FContentBrowserMenuExtender_SelectedAssets& Delegate ) { return Delegate.GetHandle() == sgContentBrowserExtenderDelegateHandle; } );
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
