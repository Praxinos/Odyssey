// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposSequenceEditorNewStoryboardDialog.h"

#include "AssetData.h"
#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "EditorStyleSet.h"
#include "Factories/Factory.h"
#include "Framework/Docking/TabManager.h"
#include "IAssetTools.h"
#include "IDetailsView.h"
#include "IStructureDetailsView.h"
#include "PropertyEditorModule.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

#include "Board/BoardSequence.h"
#include "Settings/NamingConventionSettings.h"

/* LevelSequenceEditorHelpers
 *****************************************************************************/

#define LOCTEXT_NAMESPACE "EposSequenceEditorNewStoryboardDialog"

TWeakPtr<SWindow> NewStoryboardSettingsWindow;

//---

class SNewStoryboardSettings
    : public SCompoundWidget, public FGCObject
{
    SLATE_BEGIN_ARGS( SNewStoryboardSettings )
        {}
    SLATE_END_ARGS()

    void Construct( const FArguments& InArgs );

    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;

private:
    FText GetFullPath() const;
    FText GetErrorText() const;
    bool CanCreateStoryboard() const;
    FReply OnCreateStoryboard();

private:
    TSharedPtr<IStructureDetailsView> mDetailsViewStoryboard;
    TSharedPtr<IStructureDetailsView> mDetailsViewGlobalNaming;
    TSharedPtr<IStructureDetailsView> mDetailsViewUserNaming;

    TSharedPtr<STextBlock> mErrorTextWidget;

    FStoryboardSettings mStoryboardSettings;
    UNamingConventionSettings* mNamingConventionSettings;
};

void
SNewStoryboardSettings::Construct(const FArguments& InArgs)
{
    mNamingConventionSettings = GetMutableDefault<UNamingConventionSettings>();

    FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    //---

    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.bShowOptions = false;
    DetailsViewArgs.bAllowFavoriteSystem = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    DetailsViewArgs.ViewIdentifier = "NewStoryboardSettings";

    FStructureDetailsViewArgs StructureDetailsViewArgs;

    TSharedPtr<FStructOnScope> StructOnScopeStoryboard = MakeShared<FStructOnScope>( FStoryboardSettings::StaticStruct(), (uint8*)&mStoryboardSettings );

    mDetailsViewStoryboard = PropertyEditor.CreateStructureDetailView( DetailsViewArgs, StructureDetailsViewArgs, StructOnScopeStoryboard );

    //---

    TSharedPtr<FStructOnScope> StructOnScopeGlobalNaming = MakeShared<FStructOnScope>( FNamingConventionGlobal::StaticStruct(), (uint8*)&mNamingConventionSettings->GlobalNaming );

    mDetailsViewGlobalNaming = PropertyEditor.CreateStructureDetailView( DetailsViewArgs, StructureDetailsViewArgs, StructOnScopeGlobalNaming );

    //---

    TSharedPtr<FStructOnScope> StructOnScopeUserNaming = MakeShared<FStructOnScope>( FNamingConventionUser::StaticStruct(), (uint8*)&mNamingConventionSettings->UserNaming );

    mDetailsViewUserNaming = PropertyEditor.CreateStructureDetailView( DetailsViewArgs, StructureDetailsViewArgs, StructOnScopeUserNaming );

    //---

    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4, 4, 4, 4)
        [
            SNew(SScrollBox)
            +SScrollBox::Slot()
            [
                mDetailsViewStoryboard->GetWidget().ToSharedRef()
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4, 4, 4, 4)
        [
            SNew(SScrollBox)
            +SScrollBox::Slot()
            [
                mDetailsViewGlobalNaming->GetWidget().ToSharedRef()
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4, 4, 4, 4)
        [
            SNew(SScrollBox)
            +SScrollBox::Slot()
            [
                mDetailsViewUserNaming->GetWidget().ToSharedRef()
            ]
        ]

        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .HAlign( HAlign_Right )
        .VAlign( VAlign_Bottom )
        .Padding(10.f)
        [
            SNew(STextBlock)
            .Text( this, &SNewStoryboardSettings::GetFullPath )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 5.f )
        [
            SAssignNew(mErrorTextWidget, STextBlock)
            .Text(this, &SNewStoryboardSettings::GetErrorText)
            .TextStyle( FEditorStyle::Get(), TEXT("Log.Warning") )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 5.f )
        [
            SNew(SButton)
            .ContentPadding(FMargin(10, 5))
            .Text(LOCTEXT("CreateStoryboard", "Create Storyboard"))
            .IsEnabled( this, &SNewStoryboardSettings::CanCreateStoryboard )
            .OnClicked( this, &SNewStoryboardSettings::OnCreateStoryboard)
        ]
    ];
}

void
SNewStoryboardSettings::AddReferencedObjects( FReferenceCollector& Collector ) //override
{
    Collector.AddReferencedObject( mNamingConventionSettings );
}


FText
SNewStoryboardSettings::GetFullPath() const
{
    FString FullPath = mStoryboardSettings.StoryboardPath.Path;
    FullPath /= mStoryboardSettings.StoryboardName;
    FullPath += TEXT(".uasset");

    return FText::FromString( FullPath );
}

FText
SNewStoryboardSettings::GetErrorText() const
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    FString FullPath = mStoryboardSettings.StoryboardPath.Path;
    FullPath /= mStoryboardSettings.StoryboardName;

    FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath( FName(*FullPath) );
    if (AssetData.IsValid())
        return LOCTEXT("StoryboardExists", "Warning: Storyboard Exists");

    if( mStoryboardSettings.StoryboardName.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyName", "Warning: Empty Storyboard Name" );

    if( mStoryboardSettings.StoryboardPath.Path.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyPath", "Warning: Empty Storyboard Path" );

    if( mNamingConventionSettings->GlobalNaming.StudioName.IsEmpty() || mNamingConventionSettings->GlobalNaming.StudioAccronym.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyStudioName", "Warning: Empty Studio Name or Accronym" );

    if( mNamingConventionSettings->GlobalNaming.ProductionName.IsEmpty() || mNamingConventionSettings->GlobalNaming.ProductionAccronym.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyProductionName", "Warning: Empty Production Name or Accronym" );

    return FText::GetEmpty();
}

bool
SNewStoryboardSettings::CanCreateStoryboard() const
{
    return GetErrorText().IsEmpty();
}

FReply
SNewStoryboardSettings::OnCreateStoryboard()
{
    IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );

    //---

    UObject* NewAsset = nullptr;

    // Attempt to create a new asset
    for( auto factory : AssetTools.GetNewAssetFactories() )
    {
        if( factory->CanCreateNew() && factory->ImportPriority >= 0 && factory->SupportedClass == UBoardSequence::StaticClass() )
        {
            NewAsset = AssetTools.CreateAsset( mStoryboardSettings.StoryboardName, mStoryboardSettings.StoryboardPath.Path, UBoardSequence::StaticClass(), factory );
            break;
        }
    }

    UBoardSequence* board_sequence = Cast<UBoardSequence>( NewAsset );

    if( !board_sequence )
        return FReply::Handled();

    //---

    board_sequence->NameElements.Index = INDEX_NONE; // To use the real asset name in display

    // Copy all 'global' members from settings global to board elements
    for( TFieldIterator<FProperty> settings_global_property_iterator( FNamingConventionGlobal::StaticStruct() ); settings_global_property_iterator; ++settings_global_property_iterator )
    {
        FProperty* settings_global_property = *settings_global_property_iterator;

        FProperty* board_property = FindFProperty<FProperty>( FBoardNameElements::StaticStruct(), settings_global_property->GetFName() );
        if( settings_global_property->GetName().EndsWith( TEXT( "NumDigits" ) ) )
            continue;

        check( board_property );

        // It doesn't work if the 2 structs are not synchro with the same name of members
        // and I don't know the difference with the (good) outside ContainerPtrToValuePtr<> form below
        //settings_global_property->CopyCompleteValue_InContainer( &board_sequence->NameElements, &mNamingConventionSettings->GlobalNaming );

        const uint8* SourceAddr = settings_global_property->ContainerPtrToValuePtr<uint8>( &mNamingConventionSettings->GlobalNaming );
        uint8* DestinationAddr = board_property->ContainerPtrToValuePtr<uint8>( &board_sequence->NameElements );

        settings_global_property->CopyCompleteValue( DestinationAddr, SourceAddr );
    }

    // Copy all 'user' members from settings user to board elements
    for( TFieldIterator<FProperty> settings_user_property_iterator( FNamingConventionUser::StaticStruct() ); settings_user_property_iterator; ++settings_user_property_iterator )
    {
        FProperty* settings_user_property = *settings_user_property_iterator;

        FProperty* board_property = FindFProperty<FProperty>( FBoardNameElements::StaticStruct(), settings_user_property->GetFName() );
        check( board_property );

        const uint8* SourceAddr = settings_user_property->ContainerPtrToValuePtr<uint8>( &mNamingConventionSettings->UserNaming );
        uint8* DestinationAddr = board_property->ContainerPtrToValuePtr<uint8>( &board_sequence->NameElements );

        settings_user_property->CopyCompleteValue( DestinationAddr, SourceAddr );
    }

    //---

    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset( NewAsset );

    NewStoryboardSettingsWindow.Pin()->RequestDestroyWindow();

    return FReply::Handled();
}

//---
//---
//---

void NewStoryboardDialog::OpenDialog(const TSharedRef<FTabManager>& TabManager)
{
    TSharedPtr<SWindow> ExistingWindow = NewStoryboardSettingsWindow.Pin();
    if (ExistingWindow.IsValid())
    {
        ExistingWindow->BringToFront();
    }
    else
    {
        ExistingWindow = SNew(SWindow)
            .Title( LOCTEXT("NewStoryboardDialogTitle", "New Storyboard Settings") )
            .HasCloseButton(true)
            .SupportsMaximize(false)
            .SupportsMinimize(false)
            .ClientSize(FVector2D(600, 600));

        TSharedPtr<SDockTab> OwnerTab = TabManager->GetOwnerTab();
        TSharedPtr<SWindow> RootWindow = OwnerTab.IsValid() ? OwnerTab->GetParentWindow() : TSharedPtr<SWindow>();
        if(RootWindow.IsValid())
        {
            FSlateApplication::Get().AddWindowAsNativeChild(ExistingWindow.ToSharedRef(), RootWindow.ToSharedRef());
        }
        else
        {
            FSlateApplication::Get().AddWindow(ExistingWindow.ToSharedRef());
        }
    }

    ExistingWindow->SetContent( SNew( SNewStoryboardSettings ) );

    NewStoryboardSettingsWindow = ExistingWindow;
}

#undef LOCTEXT_NAMESPACE
