// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "StoryboardCreationDialog/NewStoryboardDialog.h"

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
#include "Math/UnitConversion.h"
#include "PropertyEditorModule.h"
#include "SequencerSettings.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STileView.h"
#include "SPrimaryButton.h"

#include "Board/BoardSequence.h"
#include "IEposSequenceEditorToolkit.h"
#include "Import/ImportImageSequenceConverter.h"
#include "Import/ImportImageSequenceImporter.h"
#include "Import/ImportImageSequenceSettings.h"
#include "Import/ImportImageSequenceStruct.h"
#include "Import/SImportPanelTileView.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/NamingConventionSettings.h"
#include "StoryboardCreationDialog/StoryboardSettings.h"

/* LevelSequenceEditorHelpers
 *****************************************************************************/

#define LOCTEXT_NAMESPACE "NewStoryboardDialog"

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

    void StoryboardSettingsChanged( const FPropertyChangedEvent& iEvent );
    void ImportImageSequenceSettingsChanged( const FPropertyChangedEvent& iEvent );

    int32 GetActiveTabIndex() const;

    void MakePanelItems();
    float GetItemScaledWidth() const;
    float GetItemScaledHeight() const;
    int32 GetItemScaleMultiplier() const;
    void SetItemScaleMultiplier( int32 iItemScaleMultiplier );

    FText GetFullPath() const;
    FText GetErrorText() const;
    FText GetWarningText() const;

    bool CanCreateStoryboard() const;
    FReply OnCreateStoryboard();

private:
    EDialogType     mDialogType;

    TSharedPtr<IDetailsView>            mDetailsViewStoryboardSettings;
    TSharedPtr<IDetailsView>            mDetailsViewImportImageSequenceSettings;
    TArray<TSharedPtr<FImportPanelItem>>                mPanelItemsList;
    TSharedPtr<STileView<TSharedPtr<FImportPanelItem>>> mPanelListView;
    float                                               mItemDefaultWidth { 128.f };
    float                                               mItemDefaultHeight { 128.f };
    TSharedPtr<IDetailsView>            mDetailsViewSequencer;
    TSharedPtr<IStructureDetailsView>   mDetailsViewBoardSettings;
    TSharedPtr<IStructureDetailsView>   mDetailsViewShotSettings;
    TSharedPtr<IDetailsView>            mDetailsViewNaming;

    ETabs mActiveTab;

    UStoryboardSettings*                mStoryboardSettings;
    UImportImageSequenceSettings*       mImportImageSequenceSettings;
    UImportImageSequenceUISettings*     mImportImageSequenceUISettings;
    UNamingConventionSettings*          mNamingConventionSettings;
    UEposSequenceEditorSettings*        mSequenceEditorSettings;

    FString                             mImageSequenceImportErrorMessage;
    FImportImageSequenceStruct          mImageSequenceStruct;
};

void
SNewStoryboardSettings::Construct( const FArguments& InArgs, EDialogType iDialogType )
{
    mDialogType = iDialogType;

    mStoryboardSettings = GetMutableDefault<UStoryboardSettings>();
    if( mDialogType == EDialogType::kImportImageSequence )
    {
        mImportImageSequenceSettings = GetMutableDefault<UImportImageSequenceSettings>();
        mImportImageSequenceUISettings = GetMutableDefault<UImportImageSequenceUISettings>();
    }
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
        mDetailsViewStoryboardSettings = PropertyEditor.CreateDetailView( DetailsViewArgs );
        mDetailsViewStoryboardSettings->OnFinishedChangingProperties().AddSP( this, &SNewStoryboardSettings::StoryboardSettingsChanged );
        mDetailsViewStoryboardSettings->SetObject( mStoryboardSettings );
    }

    //---

    {
        if( mDialogType == EDialogType::kImportImageSequence )
        {
            mDetailsViewImportImageSequenceSettings = PropertyEditor.CreateDetailView( DetailsViewArgs );
            mDetailsViewImportImageSequenceSettings->OnFinishedChangingProperties().AddSP( this, &SNewStoryboardSettings::ImportImageSequenceSettingsChanged );
            mDetailsViewImportImageSequenceSettings->SetObject( mImportImageSequenceSettings );

            ImportImageSequence();
        }

        if( mDialogType == EDialogType::kImportImageSequence )
        {
            MakePanelItems();

            mPanelListView = SNew( STileView<TSharedPtr<FImportPanelItem>> )
                            .ListItemsSource( &mPanelItemsList )
                            .SelectionMode( ESelectionMode::None )
                            //.ClearSelectionOnClick( false )
                            .ItemAlignment( EListItemAlignment::LeftAligned )
                            .OnGenerateTile_Static( &SImportPanelTileView::BuildTile )
                            .ItemWidth( this, &SNewStoryboardSettings::GetItemScaledWidth )
                            .ItemHeight( this, &SNewStoryboardSettings::GetItemScaledHeight );
        }
    }

    //---

    {
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
    }

    //---

    {
        mDetailsViewNaming = PropertyEditor.CreateDetailView( DetailsViewArgs );
        mDetailsViewNaming->SetObject( mNamingConventionSettings );
    }

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

    TSharedRef<SWidgetSwitcher> switcher =
        SNew( SWidgetSwitcher )
        .WidgetIndex( this, &SNewStoryboardSettings::GetActiveTabIndex );

    if( mDialogType == EDialogType::kImportImageSequence )
    {
        switcher->AddSlot()
        [
            SNew( SVerticalBox )

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(4, 4, 4, 4)
            .HAlign( HAlign_Right )
            [
                SNew( SSpinBox<int32> )
                .TypeInterface( MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ) )
                .MinDesiredWidth( 65 )
                .Justification( ETextJustify::Right )
                .ToolTipText( LOCTEXT( "thumbnail-scale-mulitplier.tooltip", "Change the size of the thumbnails." ) )
                .MinValue( 50 )
                .MaxValue( 250 )
                .OnValueCommitted_Lambda( [=] ( int32 Value, ETextCommit::Type ) { SetItemScaleMultiplier( Value ); mPanelListView->RequestListRefresh(); } ) // RequestListRefresh() is only OnCommitted() to not refresh every mouse drags
                .OnValueChanged_Lambda( [=] ( int32 Value ) { SetItemScaleMultiplier( Value ); } )
                .Value( this, &SNewStoryboardSettings::GetItemScaleMultiplier )
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(4, 4, 4, 4)
            [
                SNew( SScrollBorder, mPanelListView.ToSharedRef() )
                [
                    mPanelListView.ToSharedRef()
                ]
            ]
        ];
    }

    switcher->AddSlot()
    [
        SNew( SVerticalBox )

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4, 4, 4, 4)
        [
            mDetailsViewNaming.ToSharedRef()
        ]
    ];

    switcher->AddSlot()
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
    ];

    switcher->AddSlot()
    [
        SNew( SVerticalBox )

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4, 4, 4, 4)
        [
            mDetailsViewSequencer.ToSharedRef()
        ]
    ];

    //---

    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 4, 4, 4, 4 )
        [
            mDetailsViewImportImageSequenceSettings.IsValid() ? mDetailsViewImportImageSequenceSettings.ToSharedRef() : SNullWidget::NullWidget
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 4, 4, 4, 4 )
        [
            mDetailsViewStoryboardSettings.ToSharedRef()
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
                switcher
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .VAlign( VAlign_Bottom )
        .Padding( 10.f, 4.f )
        [
            SNew(STextBlock)
            .Text( this, &SNewStoryboardSettings::GetFullPath )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 10.f, 4.f )
        [
            SNew(STextBlock)
            .Text(this, &SNewStoryboardSettings::GetErrorText)
            .TextStyle( FAppStyle::Get(), TEXT("Log.Error") )
            .Visibility_Lambda( [this]() { return GetErrorText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible; } )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 10.f, 4.f )
        [
            SNew( STextBlock )
            .Text(this, &SNewStoryboardSettings::GetWarningText)
            .TextStyle( FAppStyle::Get(), TEXT("Log.Warning") )
            .Visibility_Lambda( [this]() { return GetWarningText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible; } )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 10.f, 4.f, 10.f, 8.f )
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
    Collector.AddReferencedObject( mStoryboardSettings );
    if( mDialogType == EDialogType::kImportImageSequence )
    {
        Collector.AddReferencedObject( mImportImageSequenceSettings );
        Collector.AddReferencedObject( mImportImageSequenceUISettings );
    }
    Collector.AddReferencedObject( mNamingConventionSettings );
    Collector.AddReferencedObject( mSequenceEditorSettings );
}

FString
SNewStoryboardSettings::GetReferencerName() const //override
{
    return "SNewStoryboardSettings";
}

void
SNewStoryboardSettings::StoryboardSettingsChanged( const FPropertyChangedEvent& iEvent )
{
    mStoryboardSettings->SaveConfig();
}

void
SNewStoryboardSettings::ImportImageSequenceSettingsChanged( const FPropertyChangedEvent& iPropertyEvent )
{
    mImportImageSequenceSettings->SaveConfig();

    ImportImageSequence();
}

void
SNewStoryboardSettings::ImportImageSequence()
{
    mImageSequenceImportErrorMessage.Empty();

    if( mImportImageSequenceSettings->Options.ImageSequencePath.Path.IsEmpty() )
        return;

    FImportImageSequenceImporter image_sequence_importer( mImportImageSequenceSettings->Options, mImageSequenceImportErrorMessage );
    if( !mImageSequenceImportErrorMessage.IsEmpty() )
        return;

    mImageSequenceStruct = image_sequence_importer.GetImageSequenceStruct();

    mStoryboardSettings->StoryboardName = FPaths::GetBaseFilename( mImportImageSequenceSettings->Options.ImageSequencePath.Path );
}

int32
SNewStoryboardSettings::GetActiveTabIndex() const
{
    // For the moment, it's quite simple to get the active tab index
    // But if it would be more complex, use real id for mActiveTab and make them correspond to the index for the switcher
    return mDialogType == EDialogType::kImportImageSequence ? int32(mActiveTab) : int32(mActiveTab) - 1;
}

void
SNewStoryboardSettings::MakePanelItems()
{
    // Build a list of items - one for each file
    for( int32 b = 0; b < mImageSequenceStruct.Boards.Num(); b++ )
    {
        FImportImageSequenceBoard& board = mImageSequenceStruct.Boards[b];

        for( int32 s = 0; s < board.Shots.Num(); s++ )
        {
            FImportImageSequenceShot& shot = board.Shots[s];

            for( int32 f = 0; f < shot.Frames.Num(); f++ )
            {
                FImportImageSequenceFrame& frame = shot.Frames[f];

                TSharedPtr<FImportPanelItem> panel_item = MakeShareable( new FImportPanelItem() );
                panel_item->mBoardId = board.Id;
                panel_item->mShotId = shot.Id;
                panel_item->mFrame = frame;

                panel_item->mOptions = &mImportImageSequenceSettings->Options;
                panel_item->CreateThumbnail();

                mPanelItemsList.Add( panel_item );
            }
        }
    }
}

float
SNewStoryboardSettings::GetItemScaledWidth() const
{
    return mItemDefaultWidth * mImportImageSequenceUISettings->GetThumbnailScaleMultiplier() / 100.f;
}

float
SNewStoryboardSettings::GetItemScaledHeight() const
{
    return mItemDefaultHeight * mImportImageSequenceUISettings->GetThumbnailScaleMultiplier() / 100.f;
}

int32
SNewStoryboardSettings::GetItemScaleMultiplier() const
{
    return mImportImageSequenceUISettings->GetThumbnailScaleMultiplier();
}
void
SNewStoryboardSettings::SetItemScaleMultiplier( int32 iItemScaleMultiplier )
{
    mImportImageSequenceUISettings->SetThumbnailScaleMultiplier( iItemScaleMultiplier );
}

FText
SNewStoryboardSettings::GetFullPath() const
{
    FString FullPath = mStoryboardSettings->StoryboardPath.Path;
    FullPath /= mStoryboardSettings->StoryboardName;
    FullPath += TEXT(".uasset");

    return FText::FromString( FullPath );
}

FText
SNewStoryboardSettings::GetErrorText() const
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    FString FullPath = mStoryboardSettings->StoryboardPath.Path;
    FullPath /= mStoryboardSettings->StoryboardName;

    FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath( FName(*FullPath) );
    if (AssetData.IsValid())
        return LOCTEXT("StoryboardExists", "Error: Storyboard Exists");

    if( mStoryboardSettings->StoryboardName.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyName", "Error: Empty Storyboard Name" );

    if( mStoryboardSettings->StoryboardPath.Path.IsEmpty() )
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
        if( mImportImageSequenceSettings->Options.ImageSequencePath.Path.IsEmpty() )
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
            NewAsset = AssetTools.CreateAsset( mStoryboardSettings->StoryboardName, mStoryboardSettings->StoryboardPath.Path, UBoardSequence::StaticClass(), factory );
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

        FImportImageSequenceConverter( &mImageSequenceStruct, sequencer, board_sequence );
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
            .ClientSize(FVector2D(800, 900));

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
            .ClientSize( FVector2D( 800, 900 ) );

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
