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
#include "LevelEditorSequencerIntegration.h"
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
#include "IEposSequenceEditorToolkit.h"
#include "Import/ImageSequenceConverter.h"
#include "Import/ImageSequenceImporter.h"
#include "Import/ImageSequenceStruct.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/NamingConventionSettings.h"

/* LevelSequenceEditorHelpers
 *****************************************************************************/

#define LOCTEXT_NAMESPACE "EposSequenceEditorNewStoryboardDialog"

TWeakPtr<SWindow> NewStoryboardSettingsWindow;

enum class EDialogType
{
    kCreation,
    kImportImageSequence,
};

enum class ETabs
{
    // The number is the order inside SWidgetSwitcher
    kImportImageSequence = 0,
    kNamingConvention   = 1,
    kSequence           = 2,
    kSequencer          = 3,
};

//---

class SNewStoryboardSettings
    : public SCompoundWidget
    , public FGCObject
{
    SLATE_BEGIN_ARGS( SNewStoryboardSettings )
        {}
    SLATE_END_ARGS()

    void Construct( const FArguments& InArgs, EDialogType iDialogType );

    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
    virtual FString GetReferencerName() const override;

private:
    void ImportImageSequence();
    void ImportImageSequence( const FPropertyChangedEvent& iPropertyEvent );

    FText GetFullPath() const;
    FText GetErrorText() const;
    FText GetWarningText() const;

    bool CanCreateStoryboard() const;
    FReply OnCreateStoryboard();

private:
    EDialogType     mDialogType;

    TSharedPtr<IStructureDetailsView>   mDetailsViewStoryboard;
    TSharedPtr<IStructureDetailsView>   mDetailsViewStoryboardImportImageSequence;
    TSharedPtr<IStructureDetailsView>   mDetailsViewImageSequence;
    TSharedPtr<IDetailsView>            mDetailsViewSequencer;
    TSharedPtr<IStructureDetailsView>   mDetailsViewBoardSettings;
    TSharedPtr<IStructureDetailsView>   mDetailsViewShotSettings;
    TSharedPtr<IDetailsView>            mDetailsViewNaming;

    ETabs mActiveTab;

    FStoryboardSettings             mStoryboardSettings;
    FStoryboardImportImageSequenceSettings    mStoryboardImportImageSequenceSettings;
    UNamingConventionSettings*      mNamingConventionSettings;
    UEposSequenceEditorSettings*    mSequenceEditorSettings;

    FString                         mImageSequenceImportErrorMessage;
    FImageSequenceStruct            mImageSequenceStruct;
};

void
SNewStoryboardSettings::Construct( const FArguments& InArgs, EDialogType iDialogType )
{
    //DEBUG TOREMOVE
    mStoryboardImportImageSequenceSettings.ImageSequencePath.Path = FPaths::ProjectPluginsDir() + "/Epos/samples/image-sequence";
    //DEBUG TOREMOVE

    mDialogType = iDialogType;

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

    {
        if( mDialogType == EDialogType::kImportImageSequence )
        {
            TSharedPtr<FStructOnScope> StructOnScope = MakeShared<FStructOnScope>( FStoryboardImportImageSequenceSettings::StaticStruct(), (uint8*)&mStoryboardImportImageSequenceSettings );
            mDetailsViewStoryboardImportImageSequence = PropertyEditor.CreateStructureDetailView( DetailsViewArgs, StructureDetailsViewArgs, StructOnScope );
            mDetailsViewStoryboardImportImageSequence->GetOnFinishedChangingPropertiesDelegate().AddSP( this, &SNewStoryboardSettings::ImportImageSequence );

            ImportImageSequence();
        }
    }

    {
        if( mDialogType == EDialogType::kImportImageSequence )
        {
            TSharedPtr<FStructOnScope> StructOnScope = MakeShared<FStructOnScope>( FImageSequenceStruct::StaticStruct(), (uint8*)&mImageSequenceStruct );
            mDetailsViewImageSequence = PropertyEditor.CreateStructureDetailView( DetailsViewArgs, StructureDetailsViewArgs, StructOnScope );
            //mDetailsViewImageSequence->GetOnFinishedChangingPropertiesDelegate().AddSP( this, &SNewStoryboardSettings::ImportImageSequence );
        }
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

    mActiveTab = ETabs::kNamingConvention;

    if( mDialogType == EDialogType::kImportImageSequence )
    {
        mActiveTab = ETabs::kImportImageSequence;
    }

    //---

    TSharedRef<SHorizontalBox> tabs = SNew( SHorizontalBox )
                                      + SHorizontalBox::Slot()
                                      .FillWidth( .5f )
                                      [
                                          SNew( SSpacer )
                                      ];

    if( mDialogType == EDialogType::kImportImageSequence )
    {
        tabs->AddSlot()
              .HAlign( HAlign_Fill )
              .Padding( FMargin( 0.f, 1.0f, 1.0f, 0.0f ) )
              [
                  SNew(SCheckBox)
                  .Style( FAppStyle::Get(),  "ToolPalette.DockingTab" )
                  .Padding( 7.f )
                  .HAlign( HAlign_Center )
                  .OnCheckStateChanged_Lambda( [this] (const ECheckBoxState) { mActiveTab = ETabs::kImportImageSequence; } )
                  .IsChecked_Lambda( [this] () -> ECheckBoxState { return mActiveTab == ETabs::kImportImageSequence ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                  [
                      SNew( STextBlock )
                      .Text( LOCTEXT( "new-storyboard.tabs.import-image-sequence", "Images" ) )
                  ]
              ];
    }

    tabs->AddSlot()
          .HAlign( HAlign_Fill )
          .Padding( FMargin( 0.f, 1.0f, 1.0f, 0.0f ) )
          [
              SNew(SCheckBox)
              .Style( FAppStyle::Get(),  "ToolPalette.DockingTab" )
              .Padding( 7.f )
              .HAlign( HAlign_Center )
              .OnCheckStateChanged_Lambda( [this] (const ECheckBoxState) { mActiveTab = ETabs::kNamingConvention; } )
              .IsChecked_Lambda( [this] () -> ECheckBoxState { return mActiveTab == ETabs::kNamingConvention ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
              [
                  SNew( STextBlock )
                  .Text( LOCTEXT( "new-storyboard.tabs.naming", "Naming Convention" ) )
              ]
          ];

    tabs->AddSlot()
            .HAlign( HAlign_Fill )
            .Padding( FMargin( 0.f, 1.0f, 1.0f, 0.0f ) )
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToolPalette.DockingTab" )
                .Padding( 7.f )
                .HAlign( HAlign_Center )
                .OnCheckStateChanged_Lambda( [this] (const ECheckBoxState) { mActiveTab = ETabs::kSequence; } )
                .IsChecked_Lambda( [this] () -> ECheckBoxState { return mActiveTab == ETabs::kSequence ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "new-storyboard.tabs.sequence", "Sequence" ) )
                ]
            ];

    tabs->AddSlot()
            .HAlign( HAlign_Fill )
            .Padding( FMargin( 0.f, 1.0f, 1.0f, 0.0f ) )
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToolPalette.DockingTab" )
                .Padding( 7.f )
                .HAlign( HAlign_Center )
                .OnCheckStateChanged_Lambda( [this] (const ECheckBoxState) { mActiveTab = ETabs::kSequencer; } )
                .IsChecked_Lambda( [this] () -> ECheckBoxState { return mActiveTab == ETabs::kSequencer ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "new-storyboard.tabs.sequencer", "Sequencer" ) )
                ]
            ];

    tabs->AddSlot()
            .FillWidth( .5f )
            [
                SNew( SSpacer )
            ];

    //---

    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 4, 4, 4, 4 )
        [
            mDetailsViewStoryboardImportImageSequence.IsValid() ? mDetailsViewStoryboardImportImageSequence->GetWidget().ToSharedRef() : SNullWidget::NullWidget
        ]

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
            tabs
        ]

        + SVerticalBox::Slot()
        .FillHeight( 1.0f )
        [
            SNew( SScrollBox )
            + SScrollBox::Slot()
            [
                SNew( SWidgetSwitcher )
                .WidgetIndex_Lambda( [this] () -> int32 { return int32(mActiveTab); } )

                + SWidgetSwitcher::Slot()
                [
                    SNew( SVerticalBox )

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(4, 4, 4, 4)
                    [
                        mDetailsViewImageSequence.IsValid() ? mDetailsViewImageSequence->GetWidget().ToSharedRef() : SNullWidget::NullWidget
                    ]
                ]

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

void
SNewStoryboardSettings::ImportImageSequence( const FPropertyChangedEvent& iPropertyEvent )
{
    ImportImageSequence();
}

void
SNewStoryboardSettings::ImportImageSequence()
{
    mImageSequenceImportErrorMessage.Empty();

    if( mStoryboardImportImageSequenceSettings.ImageSequencePath.Path.IsEmpty() )
        return;

    FImageSequenceImporter image_sequence_importer( mStoryboardImportImageSequenceSettings.ImageSequencePath.Path, mImageSequenceImportErrorMessage );
    if( !mImageSequenceImportErrorMessage.IsEmpty() )
        return;

    mImageSequenceStruct = image_sequence_importer.GetImageSequenceStruct();

    mStoryboardSettings.StoryboardName = FPaths::GetBaseFilename( mStoryboardImportImageSequenceSettings.ImageSequencePath.Path );
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

    if( !mImageSequenceImportErrorMessage.IsEmpty() )
        return FText::FromString( mImageSequenceImportErrorMessage );

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
    if( !GetErrorText().IsEmpty() )
        return false;

    if( mDialogType == EDialogType::kImportImageSequence )
    {
        if( mStoryboardImportImageSequenceSettings.ImageSequencePath.Path.IsEmpty() )
            return false;
    }

    return true;
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

    if( mDialogType == EDialogType::kImportImageSequence )
    {
        IAssetEditorInstance* assetEditor = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset( NewAsset, false );
        IEposSequenceEditorToolkit* eposSequenceEditor = static_cast<IEposSequenceEditorToolkit*>( assetEditor );
        TSharedPtr<ISequencer> sequencer = eposSequenceEditor ? eposSequenceEditor->GetSequencer() : nullptr;
        check( sequencer.IsValid() );

        FImageSequenceConverter( &mImageSequenceStruct, sequencer, board_sequence );
    }

    //---

    NewStoryboardSettingsWindow.Pin()->RequestDestroyWindow();

    return FReply::Handled();
}

//---
//---
//---

void
NewStoryboardDialog::OpenCreationDialog(const TSharedRef<FTabManager>& TabManager)
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

    ExistingWindow->SetContent( SNew( SNewStoryboardSettings, EDialogType::kCreation ) );

    NewStoryboardSettingsWindow = ExistingWindow;
}

void
NewStoryboardDialog::OpenImportImageSequenceDialog( const TSharedRef<FTabManager>& TabManager )
{
    TSharedPtr<SWindow> ExistingWindow = NewStoryboardSettingsWindow.Pin();
    if( ExistingWindow.IsValid() )
    {
        ExistingWindow->BringToFront();
    }
    else
    {
        ExistingWindow = SNew( SWindow )
            .Title( LOCTEXT( "NewStoryboardDialogTitle", "New Storyboard Settings" ) )
            .HasCloseButton( true )
            .SupportsMaximize( false )
            .SupportsMinimize( false )
            .ClientSize( FVector2D( 600, 700 ) );

        TSharedPtr<SDockTab> OwnerTab = TabManager->GetOwnerTab();
        TSharedPtr<SWindow> RootWindow = OwnerTab.IsValid() ? OwnerTab->GetParentWindow() : TSharedPtr<SWindow>();
        if( RootWindow.IsValid() )
        {
            FSlateApplication::Get().AddWindowAsNativeChild( ExistingWindow.ToSharedRef(), RootWindow.ToSharedRef() );
        }
        else
        {
            FSlateApplication::Get().AddWindow( ExistingWindow.ToSharedRef() );
        }
    }

    ExistingWindow->SetContent( SNew( SNewStoryboardSettings, EDialogType::kImportImageSequence ) );

    NewStoryboardSettingsWindow = ExistingWindow;
}

#undef LOCTEXT_NAMESPACE
