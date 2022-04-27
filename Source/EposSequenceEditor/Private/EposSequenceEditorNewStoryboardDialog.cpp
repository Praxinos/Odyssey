// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
#include "SequencerSettings.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "SPrimaryButton.h"

#include "Board/BoardSequence.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/NamingConventionSettings.h"

/* LevelSequenceEditorHelpers
 *****************************************************************************/

#define LOCTEXT_NAMESPACE "EposSequenceEditorNewStoryboardDialog"

TWeakPtr<SWindow> NewStoryboardSettingsWindow;

//---

class SNewStoryboardSettings
    : public SCompoundWidget
    , public FGCObject
{
    SLATE_BEGIN_ARGS( SNewStoryboardSettings )
        {}
    SLATE_END_ARGS()

    void Construct( const FArguments& InArgs );

    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
    virtual FString GetReferencerName() const override;

private:
    FText GetFullPath() const;
    FText GetErrorText() const;
    FText GetWarningText() const;
    bool CanCreateStoryboard() const;
    FReply OnCreateStoryboard();

private:
    TSharedPtr<IStructureDetailsView>   mDetailsViewStoryboard;
    TSharedPtr<IDetailsView>            mDetailsViewSequencer;
    TSharedPtr<IStructureDetailsView>   mDetailsViewBoardSettings;
    TSharedPtr<IStructureDetailsView>   mDetailsViewShotSettings;
    TSharedPtr<IDetailsView>            mDetailsViewNaming;

    int32 mActiveTab { 0 };

    FStoryboardSettings             mStoryboardSettings;
    UNamingConventionSettings*      mNamingConventionSettings;
    UEposSequenceEditorSettings*    mSequenceEditorSettings;
};

void
SNewStoryboardSettings::Construct(const FArguments& InArgs)
{
    mNamingConventionSettings = GetMutableDefault<UNamingConventionSettings>();
    mSequenceEditorSettings = GetMutableDefault<UEposSequenceEditorSettings>();

    FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    //---

    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.bShowOptions = false;
    DetailsViewArgs.bAllowFavoriteSystem = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    //DetailsViewArgs.ViewIdentifier = "NewStoryboardSettings";

    FStructureDetailsViewArgs StructureDetailsViewArgs;

    {
        TSharedPtr<FStructOnScope> StructOnScope = MakeShared<FStructOnScope>( FStoryboardSettings::StaticStruct(), (uint8*)&mStoryboardSettings );
        mDetailsViewStoryboard = PropertyEditor.CreateStructureDetailView( DetailsViewArgs, StructureDetailsViewArgs, StructOnScope );
    }

    //---

    USequencerSettings* sequencer_settings = USequencerSettingsContainer::GetOrCreate<USequencerSettings>( TEXT( "EposSequencerEditor" ) );

    mDetailsViewSequencer = PropertyEditor.CreateDetailView( DetailsViewArgs );
    auto IsPropertyVisible = []( const FPropertyAndParent& iPropertyAndParent ) -> bool
    {
        if( iPropertyAndParent.Property.GetName() == TEXT( "FrameNumberDisplayFormat" ) ) // GET_MEMBER_NAME_CHECKED() can't access private members
            return true;

        return false;
    };
    mDetailsViewSequencer->SetIsPropertyVisibleDelegate( FIsPropertyVisible::CreateLambda( IsPropertyVisible ) );
    mDetailsViewSequencer->SetObject( sequencer_settings );

    //---

    mDetailsViewNaming = PropertyEditor.CreateDetailView( DetailsViewArgs );

    mDetailsViewNaming->SetObject( mNamingConventionSettings );

    //---

    {
        TSharedPtr<FStructOnScope> StructOnScope = MakeShared<FStructOnScope>( FBoardSettings::StaticStruct(), (uint8*)&mSequenceEditorSettings->BoardSettings );
        mDetailsViewBoardSettings = PropertyEditor.CreateStructureDetailView( DetailsViewArgs, StructureDetailsViewArgs, StructOnScope );
    }

    {
        TSharedPtr<FStructOnScope> StructOnScope = MakeShared<FStructOnScope>( FShotSettings::StaticStruct(), (uint8*)&mSequenceEditorSettings->ShotSettings );
        mDetailsViewShotSettings = PropertyEditor.CreateStructureDetailView( DetailsViewArgs, StructureDetailsViewArgs, StructOnScope );
    }

    //---

    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 4, 4, 4, 4 )
        [
            mDetailsViewStoryboard->GetWidget().ToSharedRef()
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Fill )
        .Padding( 4, 4, 4, 4 )
        [
            SNew( SHorizontalBox )

            + SHorizontalBox::Slot()
            .FillWidth( .5f )
            [
                SNew( SSpacer )
            ]

            + SHorizontalBox::Slot()
            .HAlign( HAlign_Fill )
            .Padding( FMargin( 0.f, 1.0f, 1.0f, 0.0f ) )
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToolPalette.DockingTab" )
                .Padding( 7.f )
                .HAlign( HAlign_Center )
                .OnCheckStateChanged_Lambda( [this] (const ECheckBoxState) { mActiveTab = 0; } )
                .IsChecked_Lambda( [this] () -> ECheckBoxState { return mActiveTab == 0 ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "new-storyboard.tabs.naming", "Naming Convention" ) )
                ]
            ]

            + SHorizontalBox::Slot()
            .HAlign( HAlign_Fill )
            .Padding( FMargin( 0.f, 1.0f, 1.0f, 0.0f ) )
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToolPalette.DockingTab" )
                .Padding( 7.f )
                .HAlign( HAlign_Center )
                .OnCheckStateChanged_Lambda( [this] (const ECheckBoxState) { mActiveTab = 1; } )
                .IsChecked_Lambda( [this] () -> ECheckBoxState { return mActiveTab == 1 ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "new-storyboard.tabs.sequence", "Sequence" ) )
                ]
            ]

            + SHorizontalBox::Slot()
            .HAlign( HAlign_Fill )
            .Padding( FMargin( 0.f, 1.0f, 1.0f, 0.0f ) )
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToolPalette.DockingTab" )
                .Padding( 7.f )
                .HAlign( HAlign_Center )
                .OnCheckStateChanged_Lambda( [this] (const ECheckBoxState) { mActiveTab = 2; } )
                .IsChecked_Lambda( [this] () -> ECheckBoxState { return mActiveTab == 2 ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "new-storyboard.tabs.sequencer", "Sequencer" ) )
                ]
            ]

            + SHorizontalBox::Slot()
            .FillWidth( .5f )
            [
                SNew( SSpacer )
            ]
        ]

        + SVerticalBox::Slot()
        .FillHeight( 1.0f )
        [
            SNew( SScrollBox )
            + SScrollBox::Slot()
            [
                SNew( SWidgetSwitcher )
                .WidgetIndex_Lambda( [this] () -> int32 { return FMath::Clamp( mActiveTab, 0, 2 ); } )

                + SWidgetSwitcher::Slot()
                [
                    SNew( SVerticalBox )

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(4, 4, 4, 4)
                    [
                        mDetailsViewNaming.ToSharedRef()
                    ]
                ]

                + SWidgetSwitcher::Slot()
                [
                    SNew( SVerticalBox )

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(4, 4, 4, 4)
                    [
                        mDetailsViewBoardSettings->GetWidget().ToSharedRef()
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(4, 4, 4, 4)
                    [
                        mDetailsViewShotSettings->GetWidget().ToSharedRef()
                    ]
                ]

                + SWidgetSwitcher::Slot()
                [
                    SNew( SVerticalBox )

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(4, 4, 4, 4)
                    [
                        mDetailsViewSequencer.ToSharedRef()
                    ]
                ]
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .VAlign( VAlign_Bottom )
        .Padding( 10.f )
        [
            SNew(STextBlock)
            .Text( this, &SNewStoryboardSettings::GetFullPath )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 2.f )
        [
            SNew(STextBlock)
            .Text(this, &SNewStoryboardSettings::GetErrorText)
            .TextStyle( FAppStyle::Get(), TEXT("Log.Error") )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 2.f )
        [
            SNew( STextBlock )
            .Text(this, &SNewStoryboardSettings::GetWarningText)
            .TextStyle( FAppStyle::Get(), TEXT("Log.Warning") )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 5.f )
        [
            SNew( SPrimaryButton )
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
    Collector.AddReferencedObject( mSequenceEditorSettings );
}

FString
SNewStoryboardSettings::GetReferencerName() const //override
{
    return "SNewStoryboardSettings";
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
        return LOCTEXT("StoryboardExists", "Error: Storyboard Exists");

    if( mStoryboardSettings.StoryboardName.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyName", "Error: Empty Storyboard Name" );

    if( mStoryboardSettings.StoryboardPath.Path.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyPath", "Error: Empty Storyboard Path" );

    if( mNamingConventionSettings->GlobalNaming.StudioName.IsEmpty() || mNamingConventionSettings->GlobalNaming.StudioAcronym.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyStudioName", "Error: Empty Studio Name or Acronym" );

    if( mNamingConventionSettings->GlobalNaming.ProductionName.IsEmpty() || mNamingConventionSettings->GlobalNaming.ProductionAcronym.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyProductionName", "Error: Empty Production Name or Acronym" );

    return FText::GetEmpty();
}

FText
SNewStoryboardSettings::GetWarningText() const
{
    if( mSequenceEditorSettings->BoardSettings.DefaultTickFrameRate.AsDecimal() < 1000.f
        || mSequenceEditorSettings->ShotSettings.DefaultTickFrameRate.AsDecimal() < 1000.f )
        return LOCTEXT( "SequenceTickFramerateTooLow", "Warning: 'Default Tick Framerate' seems to be too low.\n'Default Tick Framerate' should be 'Default Display Framerate' * 1000\ne.g. 'Default Display Framerate' = 12fps -> 'Default Tick Framerate' = 12000fps" );

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
            .ClientSize(FVector2D(600, 700));

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
