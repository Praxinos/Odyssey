// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyBrush_ContentBrowserExtensions.h"

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
#include "PropertyCustomizationHelpers.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyBrushThumbnailRenderer.h"
#include "ObjectTools.h"

#include "OdysseyBrushEditorModule.h"

#define LOCTEXT_NAMESPACE "OdysseyBrush_ContentBrowserExtensions"


/////////////////////////////////////////////////////
// SOdysseyThumbnailSelectWindow
class SOdysseyThumbnailSelectWindow
    : public SWindow
{
public:
    ~SOdysseyThumbnailSelectWindow();
    void Construct( const FArguments& iArgs );

    bool GetWindowAnswer();

public:
    FReply OnAccept();
    FReply OnCancel();
    FString ObjectPath() const;
    void OnObjectChanged(const FAssetData& AssetData);
    
    UTexture* GetCurrentTexture();

private:
    bool mWindowAnswer;
    UTexture* mCurrentTexture;
    TSharedPtr< FAssetThumbnailPool > mAssetThumbnailPool;
};

SOdysseyThumbnailSelectWindow::~SOdysseyThumbnailSelectWindow()
{
    mAssetThumbnailPool.Reset();
}

void
SOdysseyThumbnailSelectWindow::Construct( const FArguments& iArgs )
{
    mWindowAnswer = false;
    
    mAssetThumbnailPool = MakeShareable( new FAssetThumbnailPool( 1024 ) );

    SWindow::Construct( SWindow::FArguments()
        .Title( LOCTEXT( "SelectThumbnailOdysseyBrush", "Select Thumbnail for Brush" ) )
        .SizingRule( ESizingRule::FixedSize )
        .ClientSize( FVector2D( 320, 64 ) )
        .SupportsMinimize( false )
        .SupportsMaximize( false )
        [
            SNew( SBorder )
            .BorderImage( FEditorStyle::GetBrush( "Menu.Background" ) )
            [
                SNew( SVerticalBox )
                +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SObjectPropertyEntryBox)
                    .AllowedClass(          UTexture::StaticClass() )
                    .ObjectPath(            this, &SOdysseyThumbnailSelectWindow::ObjectPath )
                    .ThumbnailPool(         mAssetThumbnailPool )
                    .OnObjectChanged(       this, &SOdysseyThumbnailSelectWindow::OnObjectChanged )
                    .AllowClear(            true )
                    .DisplayUseSelected(    false )
                    .DisplayBrowse(         false )
                    .DisplayThumbnail(      false )
                    .EnableContentPicker(   true )
                    .DisplayCompactSize(    true )
                    .DisplayThumbnail(      true )
                    .ThumbnailSizeOverride( FIntPoint( 32, 32 ) )
                ]
                +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    [
                        SNew( SButton )
                        .Text( LOCTEXT( "Switch Thumbnail", "Switch Thumbnail" ) )
                        .HAlign( HAlign_Center )
                        .OnClicked_Raw( this, &SOdysseyThumbnailSelectWindow::OnAccept )
                    ]
                    +SHorizontalBox::Slot()
                    [
                        SNew( SButton )
                        .Text( LOCTEXT( "Cancel", "Cancel" ) )
                        .HAlign( HAlign_Center )
                        .OnClicked_Raw( this, &SOdysseyThumbnailSelectWindow::OnCancel )
                    ]
                ]
            ]
        ]
    );
}

bool SOdysseyThumbnailSelectWindow::GetWindowAnswer()
{
    return mWindowAnswer;
}


FReply
SOdysseyThumbnailSelectWindow::OnAccept()
{
    mWindowAnswer = true;
    RequestDestroyWindow();

    return FReply::Handled();
}

FReply
SOdysseyThumbnailSelectWindow::OnCancel()
{
    mWindowAnswer = false;
    RequestDestroyWindow();

    return FReply::Handled();
}

FString SOdysseyThumbnailSelectWindow::ObjectPath() const
{
    if( !mCurrentTexture )
        return FString();

    return mCurrentTexture->GetPathName();
}


void SOdysseyThumbnailSelectWindow::OnObjectChanged( const FAssetData& AssetData )
{
    if( AssetData.IsValid() )
        mCurrentTexture = CastChecked< UTexture >( AssetData.GetAsset() );
}

UTexture* SOdysseyThumbnailSelectWindow::GetCurrentTexture()
{
    return mCurrentTexture;
}

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
// FEditThumbnailExtension
struct FEditThumbnailExtension
    : public FContentBrowserSelectedAssetExtensionBase
{
public:
    FEditThumbnailExtension();

public:
    virtual void Execute() override;

    void EditThumbnails( TArray<UOdysseyBrush*>& iBrushes );
};

//---

FEditThumbnailExtension::FEditThumbnailExtension()
    : FContentBrowserSelectedAssetExtensionBase()
{
}

void
FEditThumbnailExtension::Execute()
{
    TArray<UOdysseyBrush*> brushes;
    for( auto assetIt = mSelectedAssets.CreateConstIterator(); assetIt; ++assetIt )
    {
        const FAssetData& assetData = *assetIt;
        if( UOdysseyBrush* brush = Cast<UOdysseyBrush>( assetData.GetAsset() ) )
        {
            brushes.Add( brush );
        }
    }

    EditThumbnails( brushes );
}

void
FEditThumbnailExtension::EditThumbnails( TArray<UOdysseyBrush*>& iBrushes )
{
    TSharedPtr<SOdysseyThumbnailSelectWindow> thumbnailSelectionWindow = SNew( SOdysseyThumbnailSelectWindow );
    GEditor->EditorAddModalWindow( thumbnailSelectionWindow.ToSharedRef() );
    
    if( thumbnailSelectionWindow->GetWindowAnswer() )
    {
        for( int i = 0; i < iBrushes.Num(); i++ )
        {
            UObject* object = thumbnailSelectionWindow->GetCurrentTexture();
            if( object )
            {
                FObjectThumbnail thumbnail;
                ThumbnailTools::AssetHasCustomThumbnail( object->GetFullName(), thumbnail );
                
                if( thumbnail.GetImageWidth() != 0 && thumbnail.GetImageHeight() != 0 )
                {
                    FObjectThumbnail* newThumbnail = ThumbnailTools::CacheThumbnail( iBrushes[i]->GetFullName(), &thumbnail, iBrushes[i]->GetOutermost() );
                    
                    UPackage* package = iBrushes[i]->GetOutermost();
                    
                    newThumbnail->MarkAsDirty();
                    package->MarkPackageDirty();
                    iBrushes[i]->PostEditChange();
            
                    newThumbnail->SetCreatedAfterCustomThumbsEnabled();
                }
            }
            else
            {
                ThumbnailTools::CacheEmptyThumbnail( iBrushes[i]->GetFullName(), iBrushes[i]->GetOutermost() );

                //Refresh the thumbnail
                UPackage* package = iBrushes[i]->GetOutermost();

                UPackage::Save(iBrushes[i]->GetOutermost(), iBrushes[i], iBrushes[i]->GetFlags(), *(iBrushes[i]->GetName()));
            
                package->SetDirtyFlag( true );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushContentBrowserExtensions_Impl
class FOdysseyBrushContentBrowserExtensions_Impl
{
public:
    static void ExecuteSelectedContentFunctor( TSharedPtr<FContentBrowserSelectedAssetExtensionBase> iSelectedAssetFunctor );

    static void PopulateBrushActionsMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets );

    static TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu( const TArray<FAssetData>& iSelectedAssets );

    static TArray<FContentBrowserMenuExtender_SelectedAssets>& GetExtenderDelegates();
};

//---

//static
void
FOdysseyBrushContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor( TSharedPtr<FContentBrowserSelectedAssetExtensionBase> iSelectedAssetFunctor )
{
    iSelectedAssetFunctor->Execute();
}

//static
void
FOdysseyBrushContentBrowserExtensions_Impl::PopulateBrushActionsMenu( FMenuBuilder& ioMenuBuilder, TArray<FAssetData> iSelectedAssets )
{
    TSharedPtr<FEditThumbnailExtension> editBrushFunctor = MakeShareable( new FEditThumbnailExtension() );
    editBrushFunctor->mSelectedAssets = iSelectedAssets;

    FUIAction action_EditBrush(
        FExecuteAction::CreateStatic( &FOdysseyBrushContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor, StaticCastSharedPtr<FContentBrowserSelectedAssetExtensionBase>( editBrushFunctor ) ) );

    ioMenuBuilder.AddMenuEntry(
          LOCTEXT( "CB_Extension_Brush_EditThumbnail", "Edit Thumbnail" )
        , LOCTEXT( "CB_Extension_Brush_EditThumbnail_Tooltip", "Change the thumbnail by the content of a texture" )
        , FSlateIcon( "OdysseyStyle", "BrushEditor.EditThumbnail16" )
        , action_EditBrush
        , NAME_None
        , EUserInterfaceActionType::Button );
}

//static
TSharedRef<FExtender>
FOdysseyBrushContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu( const TArray<FAssetData>& iSelectedAssets )
{
    TSharedRef<FExtender> extender( new FExtender() );
    
    bool isAnyBrushs = false;
    for( auto assetIt = iSelectedAssets.CreateConstIterator(); assetIt; ++assetIt )
    {
        const FAssetData& asset = *assetIt;
        isAnyBrushs = isAnyBrushs || ( asset.AssetClass == UOdysseyBrush::StaticClass()->GetFName() );
    }

    if( isAnyBrushs )
    {
        // Add the edit Brush action to the menu
        extender->AddMenuExtension(
              "GetAssetActions"
            , EExtensionHook::After
            , nullptr
            , FMenuExtensionDelegate::CreateStatic( &FOdysseyBrushContentBrowserExtensions_Impl::PopulateBrushActionsMenu, iSelectedAssets ) );
    }
    return extender;
}

//static
TArray<FContentBrowserMenuExtender_SelectedAssets>&
FOdysseyBrushContentBrowserExtensions_Impl::GetExtenderDelegates()
{
    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );

    return contentBrowserModule.GetAllAssetViewContextMenuExtenders();
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushContentBrowserExtensions
//static
void
FOdysseyBrushContentBrowserExtensions::InstallHooks()
{
    sgContentBrowserExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic( &FOdysseyBrushContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu );

    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = FOdysseyBrushContentBrowserExtensions_Impl::GetExtenderDelegates();
    cbMenuExtenderDelegates.Add( sgContentBrowserExtenderDelegate );
    sgContentBrowserExtenderDelegateHandle = cbMenuExtenderDelegates.Last().GetHandle();
}

//static
void
FOdysseyBrushContentBrowserExtensions::RemoveHooks()
{
    TArray<FContentBrowserMenuExtender_SelectedAssets>& cbMenuExtenderDelegates = FOdysseyBrushContentBrowserExtensions_Impl::GetExtenderDelegates();
    cbMenuExtenderDelegates.RemoveAll( []( const FContentBrowserMenuExtender_SelectedAssets& Delegate ) { return Delegate.GetHandle() == sgContentBrowserExtenderDelegateHandle; } );
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
