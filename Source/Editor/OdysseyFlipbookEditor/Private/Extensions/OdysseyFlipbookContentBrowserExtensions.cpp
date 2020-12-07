// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookContentBrowserExtensions.h"

#include "AssetData.h"
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

#define LOCTEXT_NAMESPACE "OdysseyFlipbookContentBrowserExtensions"

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
EditFlipbooksWarning()
{
    FText Title = LOCTEXT("TitleDeletingCurrentLayer", "Flipbook Already Opened");
    FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("DeletingCurrentLayer", "The flipbook or one of its sprite or textures is already opened in an other editor. Please close the editor before opening the flipbook with ILIAD."), &Title);
}

void
FEditFlipbookExtension::EditFlipbooks( TArray<UPaperFlipbook*>& iFlipbooks )
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    bool warningDisplayed = false;
    for( auto FlipbookIt = iFlipbooks.CreateConstIterator(); FlipbookIt; ++FlipbookIt )
    {
		UPaperFlipbook* Flipbook = *FlipbookIt;

		//PATCH: To avoid opening ILIAD when another editor for this asset is opened
		// To make it right, we should use AssetEditorSubsystem->OpenEditorForAsset, but for now it would call the default editor instead of ILIAD
        if (AssetEditorSubsystem->FindEditorForAsset(Flipbook, true) != nullptr)
            continue;

        bool editorFound = false;

        for (int i = 0; i < Flipbook->GetNumKeyFrames(); i++)
        {
            UPaperSprite* sprite = Flipbook->GetKeyFrameChecked(i).Sprite;
            if (!sprite)
                continue;

            if (AssetEditorSubsystem->FindEditorForAsset(sprite, true) != nullptr)
            {
                editorFound = true;
                break;
            }

            UTexture2D* texture = sprite->GetSourceTexture();
            if (!texture)
                continue;

            if (AssetEditorSubsystem->FindEditorForAsset(texture, true) != nullptr)
            {
                editorFound = true;
                break;
            }
        }

        if (editorFound)
        {
            if (!warningDisplayed)
            {
                EditFlipbooksWarning();
                warningDisplayed = true;
            }
            continue;
        }


        IOdysseyFlipbookEditorModule* odysseyFlipbookEditorModule = &FModuleManager::GetModuleChecked<IOdysseyFlipbookEditorModule>( "OdysseyFlipbookEditor" );
        odysseyFlipbookEditorModule->CreateOdysseyFlipbookEditor( EToolkitMode::Standalone, NULL, Flipbook );
    }
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
          LOCTEXT( "CB_Extension_Flipbook_OpenPaintEditor", "Edit Flipbook with ILIAD" )
        , LOCTEXT( "CB_Extension_Flipbook_OpenPaintEditor_Tooltip", "Open ILIAD paint editor for the selected Flipbook" )
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
        isAnyFlipbooks = isAnyFlipbooks || ( asset.AssetClass == UPaperFlipbook::StaticClass()->GetFName() );
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
