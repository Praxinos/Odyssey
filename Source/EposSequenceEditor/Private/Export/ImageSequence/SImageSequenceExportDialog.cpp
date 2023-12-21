// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ImageSequence/SImageSequenceExportDialog.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "EditorStyleSet.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Factories/Factory.h"
#include "FrameNumberDetailsCustomization.h"
#include "Framework/Docking/TabManager.h"
#include "IAssetTools.h"
#include "IDetailsView.h"
#include "ImageWrapperHelper.h"
#include "ISequencer.h"
#include "IStructureDetailsView.h"
#include "LevelEditorSequencerIntegration.h"
#include "Math/UnitConversion.h"
#include "MovieSceneSequenceVisitor.h"
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
#include "Export/ExportConverter.h"
#include "Export/ExportStruct.h"
#include "Export/ImageSequence/ExportImageSequenceExporter.h"
#include "Export/ImageSequence/ExportImageSequenceSettings.h"
#include "Export/ImageSequence/SExportPanelTileView.h"
#include "Export/SceneRenderer.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/NamingConventionSettings.h"

/* LevelSequenceEditorHelpers
 *****************************************************************************/

#define LOCTEXT_NAMESPACE "ImageSequenceExportDialog"

//---

void
SExportImageSequenceSettings::Construct( const FArguments& InArgs, TWeakPtr<ISequencer> iSequencer, FMovieSceneSequenceIDRef iSequenceId )
{
    mSequencer = iSequencer;
    mCurrentSequenceId = iSequenceId;
    mCurrentSequence = mSequencer.Pin()->GetEvaluationTemplate().GetSequence( mCurrentSequenceId );
    mCurrentEposSequence = Cast<UEposMovieSceneSequence>( mCurrentSequence );
    mRootEposSequence = EposSequenceHelpers::GetRootEposSequence( *mSequencer.Pin(), iSequenceId, mRootEposSequenceId );

    mExportImageSequenceSettings = GetMutableDefault<UExportImageSequenceSettings>();
    mExportImageSequenceUISettings = GetMutableDefault<UExportImageSequenceUISettings>();

    // Force initialize the ratio from the more relevant camera
    mExportImageSequenceSettings->Options.AspectRatio = GetMostRelevantCameraAspectRatio( mSequencer.Pin().Get(), mRootEposSequence ? mRootEposSequenceId : mCurrentSequenceId );
    mExportImageSequenceSettings->Options.ImageSize.X = mExportImageSequenceSettings->Options.ImageSize.Y * mExportImageSequenceSettings->Options.AspectRatio;

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

    //---

    {
        mDetailsViewExportImageSequenceSettings = PropertyEditor.CreateDetailView( DetailsViewArgs );
        mDetailsViewExportImageSequenceSettings->OnFinishedChangingProperties().AddSP( this, &SExportImageSequenceSettings::GlobalSettingsChanged );
        mDetailsViewExportImageSequenceSettings->SetObject( mExportImageSequenceSettings );
    }

    {
        MakePanelItems();

        mPanelListView = SNew( STileView<TSharedPtr<FPanelItem>> )
                        .ListItemsSource( &mPanelItemsList )
                        .SelectionMode( ESelectionMode::None )
                        //.ClearSelectionOnClick( false )
                        .ItemAlignment( EListItemAlignment::LeftAligned )
                        .OnGenerateTile_Static( &SPanelTileView::BuildTile )
                        .ItemWidth( this, &SExportImageSequenceSettings::GetItemScaledWidth )
                        .ItemHeight( this, &SExportImageSequenceSettings::GetItemScaledHeight );
    }

    //---

    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 4, 4, 4, 4 )
        [
            mDetailsViewExportImageSequenceSettings.ToSharedRef()
        ]

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
            .OnValueCommitted_Lambda( [this] ( int32 Value, ETextCommit::Type ) { SetItemScaleMultiplier( Value ); mPanelListView->RequestListRefresh(); } ) // RequestListRefresh() is only OnCommitted() to not refresh every mouse drags
            .OnValueChanged_Lambda( [this] ( int32 Value ) { SetItemScaleMultiplier( Value ); } )
            .Value( this, &SExportImageSequenceSettings::GetItemScaleMultiplier )
        ]

        + SVerticalBox::Slot()
        .FillHeight( 1.0f )
        .Padding(4, 4, 4, 4)
        [
            SNew( SScrollBorder, mPanelListView.ToSharedRef() )
            [
                mPanelListView.ToSharedRef()
            ]
        ]
    ];
}

//---

void
SExportImageSequenceSettings::AddReferencedObjects( FReferenceCollector& Collector ) //override
{
    Collector.AddReferencedObject( mExportImageSequenceSettings );
    Collector.AddReferencedObject( mExportImageSequenceUISettings );
}

FString
SExportImageSequenceSettings::GetReferencerName() const //override
{
    return "SExportImageSequenceSettings";
}

//---

void
SExportImageSequenceSettings::GlobalSettingsChanged( const FPropertyChangedEvent& iEvent )
{
    mExportImageSequenceSettings->SaveConfig();

    // Update panel list only for relevent options (inside marks for the moment)
    // Other settings won't change the panel list
    if( iEvent.Property->GetOwnerStruct()
        && iEvent.Property->GetOwnerStruct()->GetFName() == FExportMarkSettings::StaticStruct()->GetFName() )
    {
        if( mPanelListView )
        {
            MakePanelItems();
            mPanelListView->RequestListRefresh();
        }
    }
}

void
SExportImageSequenceSettings::MakePanelItems()
{
    mPanelItemsList.Empty();

    FExportStruct image_sequence_struct;
    FExportConverter converter( mSequencer, mRootEposSequence ? mRootEposSequenceId : mCurrentSequenceId, &mExportImageSequenceSettings->Options.MarkSettings, &image_sequence_struct );

    // Build a list of items - one for each panel
    for( int32 i = 0; i < image_sequence_struct.Panels.Num(); i++ )
    {
        TSharedPtr<FPanelItem> panel_item = MakeShareable( new FPanelItem() );
        panel_item->mPanel = image_sequence_struct.Panels[i];
        panel_item->mIndex = i;
        panel_item->mExport = true;
        panel_item->mSequencer = mSequencer;
        panel_item->mOptions = &mExportImageSequenceSettings->Options;
        panel_item->CreateThumbnail();

        mPanelItemsList.Add( panel_item );
    }
}

float
SExportImageSequenceSettings::GetItemScaledWidth() const
{
    return mItemDefaultWidth * mExportImageSequenceUISettings->GetThumbnailScaleMultiplier() / 100.f;
}

float
SExportImageSequenceSettings::GetItemScaledHeight() const
{
    return mItemDefaultHeight * mExportImageSequenceUISettings->GetThumbnailScaleMultiplier() / 100.f;
}

int32
SExportImageSequenceSettings::GetItemScaleMultiplier() const
{
    return mExportImageSequenceUISettings->GetThumbnailScaleMultiplier();
}
void
SExportImageSequenceSettings::SetItemScaleMultiplier( int32 iItemScaleMultiplier )
{
    mExportImageSequenceUISettings->SetThumbnailScaleMultiplier( iItemScaleMultiplier );
}

FText
SExportImageSequenceSettings::GetFullPath() const
{
    FString FullPath = mExportImageSequenceSettings->Options.ExportPath.Path;
    FullPath /= mExportImageSequenceSettings->Options.Pattern;
    FullPath += ImageWrapperHelper::GetFormatExtension( EImageFormat( mExportImageSequenceSettings->Options.FileFormat ), true );

    return FText::FromString( FullPath );
}

FText
SExportImageSequenceSettings::GetErrorText() const
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    if( mExportImageSequenceSettings->Options.Pattern.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyName", "Error: Empty Storyboard Pattern" );

    if( mExportImageSequenceSettings->Options.ExportPath.Path.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyPath", "Error: Empty Storyboard Path" );

    if( !mExportImageSequenceSettings->Options.mPatternKeywordLists.IsValidPattern( mExportImageSequenceSettings->Options.Pattern ) )
        return LOCTEXT( "WrongPattern", "Error: Wrong Pattern" );

    return FText::GetEmpty();
}

FText
SExportImageSequenceSettings::GetWarningText() const
{
    return FText::GetEmpty();
}

bool
SExportImageSequenceSettings::CanExportStoryboard() const
{
    if( !GetErrorText().IsEmpty() )
        return false;

    if( mExportImageSequenceSettings->Options.ExportPath.Path.IsEmpty() )
        return false;

    return true;
}

void
SExportImageSequenceSettings::ExportStoryboard()
{
    FExportStruct image_sequence_struct;
    for( const auto& panel : mPanelItemsList )
    {
        if( !panel->mExport )
            continue;

        image_sequence_struct.Panels.Add( panel->mPanel );
    }

    image_sequence_struct.mSequencer = mSequencer;

    FExportImageSequenceExporter exporter( mSequencer, &image_sequence_struct, &mExportImageSequenceSettings->Options );
    exporter.Export();
}

#undef LOCTEXT_NAMESPACE
