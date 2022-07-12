// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/SImageSequenceExportDialog.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "EditorStyleSet.h"
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
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

/* LevelSequenceEditorHelpers
 *****************************************************************************/

#define LOCTEXT_NAMESPACE "ImageSequenceExportDialog"

//---

class SExportStoryboardSettings
    : public SCompoundWidget
    , public FGCObject
{
    SLATE_BEGIN_ARGS( SExportStoryboardSettings )
        {}
        SLATE_ARGUMENT( TSharedPtr<SWindow>, ParentWindow )
    SLATE_END_ARGS()

    void Construct( const FArguments& InArgs, TWeakPtr<ISequencer> iSequencer, UMovieSceneSequence* iCurrentSequence );

public:
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
    virtual FString GetReferencerName() const override;

private:
    virtual FReply OnKeyDown( const FGeometry& iMyGeometry, const FKeyEvent& iKeyEvent ) override;

private:
    float GetMostRelevantCameraAspectRatio() const;

    void GlobalSettingsChanged( const FPropertyChangedEvent& iEvent );

    void MakePanelItems();
    float GetItemScaledWidth() const;
    float GetItemScaledHeight() const;
    int32 GetItemScaleMultiplier() const;
    void SetItemScaleMultiplier( int32 iItemScaleMultiplier );

    FText GetFullPath() const;
    FText GetErrorText() const;
    FText GetWarningText() const;

    bool CanExportStoryboard() const;
    FReply OnExportStoryboard();

private:
    TWeakPtr<SWindow>           mParentWindow;

    TWeakPtr<ISequencer>        mSequencer;
    UMovieSceneSequence*        mRootSequence;
    UMovieSceneSequence*        mCurrentSequence;

    TSharedPtr<IDetailsView>                        mDetailsViewExportImageSequenceSettings;
    TArray<TSharedPtr<FPanelItem>>                  mPanelItemsList;
    TSharedPtr<STileView<TSharedPtr<FPanelItem>>>   mPanelListView;
    float                                           mItemDefaultWidth { 192.f };
    float                                           mItemDefaultHeight { 192.f };

    UExportImageSequenceSettings*                   mExportImageSequenceSettings;
    UExportImageSequenceUISettings*                 mExportImageSequenceUISettings;

    FString                                         mImageSequenceExportErrorMessage;
};

void
SExportStoryboardSettings::Construct( const FArguments& InArgs, TWeakPtr<ISequencer> iSequencer, UMovieSceneSequence* iCurrentSequence )
{
    mParentWindow = InArgs._ParentWindow;
    check( mParentWindow.IsValid() );

    mSequencer = iSequencer;
    mCurrentSequence = iCurrentSequence;
    mRootSequence = iSequencer.Pin()->GetRootMovieSceneSequence();

    mExportImageSequenceSettings = GetMutableDefault<UExportImageSequenceSettings>();
    mExportImageSequenceUISettings = GetMutableDefault<UExportImageSequenceUISettings>();

    // Force initialize the ratio from the more relevant camera
    mExportImageSequenceSettings->Options.AspectRatio = GetMostRelevantCameraAspectRatio();
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
        mDetailsViewExportImageSequenceSettings->OnFinishedChangingProperties().AddSP( this, &SExportStoryboardSettings::GlobalSettingsChanged );
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
                        .ItemWidth( this, &SExportStoryboardSettings::GetItemScaledWidth )
                        .ItemHeight( this, &SExportStoryboardSettings::GetItemScaledHeight );
    }

    //---


    TSharedRef<SPrimaryButton> export_panels = SNew( SPrimaryButton )
                                               .Text( LOCTEXT( "ExportStoryboard", "Export Panels" ) )
                                               .IsEnabled( this, &SExportStoryboardSettings::CanExportStoryboard )
                                               .OnClicked( this, &SExportStoryboardSettings::OnExportStoryboard );

    mParentWindow.Pin().Get()->SetWidgetToFocusOnActivate( export_panels );

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
            .OnValueCommitted_Lambda( [=] ( int32 Value, ETextCommit::Type ) { SetItemScaleMultiplier( Value ); mPanelListView->RequestListRefresh(); } ) // RequestListRefresh() is only OnCommitted() to not refresh every mouse drags
            .OnValueChanged_Lambda( [=] ( int32 Value ) { SetItemScaleMultiplier( Value ); } )
            .Value( this, &SExportStoryboardSettings::GetItemScaleMultiplier )
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

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .VAlign( VAlign_Bottom )
        .Padding( 10.f, 4.f )
        [
            SNew(STextBlock)
            .Text( this, &SExportStoryboardSettings::GetFullPath )
            .AutoWrapText( true )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 10.f, 4.f )
        [
            SNew(STextBlock)
            .Text(this, &SExportStoryboardSettings::GetErrorText)
            .TextStyle( FAppStyle::Get(), TEXT("Log.Error") )
            .Visibility_Lambda( [this]() { return GetErrorText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible; } )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 10.f, 4.f )
        [
            SNew( STextBlock )
            .Text(this, &SExportStoryboardSettings::GetWarningText)
            .TextStyle( FAppStyle::Get(), TEXT("Log.Warning") )
            .Visibility_Lambda( [this]() { return GetWarningText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible; } )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 10.f, 4.f, 10.f, 8.f )
        [
            export_panels
        ]
    ];
}

//---

void
SExportStoryboardSettings::AddReferencedObjects( FReferenceCollector& Collector ) //override
{
    Collector.AddReferencedObject( mExportImageSequenceSettings );
    Collector.AddReferencedObject( mExportImageSequenceUISettings );
}

FString
SExportStoryboardSettings::GetReferencerName() const //override
{
    return "SExportStoryboardSettings";
}

//---

FReply
SExportStoryboardSettings::OnKeyDown( const FGeometry& iMyGeometry, const FKeyEvent& iKeyEvent ) //override
{
    if( iKeyEvent.GetKey() == EKeys::Escape )
    {
        mParentWindow.Pin()->RequestDestroyWindow();

        return FReply::Handled();
    }

    return SCompoundWidget::OnKeyDown( iMyGeometry, iKeyEvent );
}

//---

float
SExportStoryboardSettings::GetMostRelevantCameraAspectRatio() const
{
    struct FSequenceCameraVisitor
        : UE::MovieScene::ISequenceVisitor
    {
        virtual void VisitSection( UMovieSceneTrack* iTrack, UMovieSceneSection* iSection, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace )
        {
            UMovieSceneSingleCameraCutSection* cameracut_section = Cast<UMovieSceneSingleCameraCutSection>( iSection );
            if( !cameracut_section )
                return;

            UCameraComponent* camera_component = cameracut_section->GetFirstCamera( *mSequencer, iLocalSpace.SequenceID );
            if( !camera_component )
                return;

            FMinimalViewInfo ViewInfo;
            camera_component->GetCameraView( FApp::GetDeltaTime(), ViewInfo );

            int32* count = mAspectRatios.Find( ViewInfo.AspectRatio );
            if( count )
                *count = *count + 1;
            else
                mAspectRatios.Add( ViewInfo.AspectRatio, 1 );
        }

        ISequencer* mSequencer;
        TMap<float, int32>  mAspectRatios;
    };

    //---

    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitSections = true;
    params.bVisitMasterTracks = true;
    params.bVisitSubSequences = true;

    FSequenceCameraVisitor camera_visitor;
    camera_visitor.mSequencer = mSequencer.Pin().Get();

    // Visit all notes
    VisitSequence( mRootSequence, params, camera_visitor );

    //---

    camera_visitor.mAspectRatios.ValueSort( TGreater<int32>() );

    TArray<float> keys;
    camera_visitor.mAspectRatios.GetKeys( keys );

    return keys.Num() ? keys[0] : 1.77777f;
}

void
SExportStoryboardSettings::GlobalSettingsChanged( const FPropertyChangedEvent& iEvent )
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
SExportStoryboardSettings::MakePanelItems()
{
    mPanelItemsList.Empty();

    FExportStruct image_sequence_struct;
    FExportConverter converter( mSequencer, mRootSequence, &mExportImageSequenceSettings->Options.MarkSettings, &image_sequence_struct );

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
SExportStoryboardSettings::GetItemScaledWidth() const
{
    return mItemDefaultWidth * mExportImageSequenceUISettings->GetThumbnailScaleMultiplier() / 100.f;
}

float
SExportStoryboardSettings::GetItemScaledHeight() const
{
    return mItemDefaultHeight * mExportImageSequenceUISettings->GetThumbnailScaleMultiplier() / 100.f;
}

int32
SExportStoryboardSettings::GetItemScaleMultiplier() const
{
    return mExportImageSequenceUISettings->GetThumbnailScaleMultiplier();
}
void
SExportStoryboardSettings::SetItemScaleMultiplier( int32 iItemScaleMultiplier )
{
    mExportImageSequenceUISettings->SetThumbnailScaleMultiplier( iItemScaleMultiplier );
}

FText
SExportStoryboardSettings::GetFullPath() const
{
    FString FullPath = mExportImageSequenceSettings->Options.ExportPath.Path;
    FullPath /= mExportImageSequenceSettings->Options.Pattern;
    FullPath += ImageWrapperHelper::GetFormatExtension( EImageFormat( mExportImageSequenceSettings->Options.FileFormat ), true );

    return FText::FromString( FullPath );
}

FText
SExportStoryboardSettings::GetErrorText() const
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    if( mExportImageSequenceSettings->Options.Pattern.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyName", "Error: Empty Storyboard Pattern" );

    if( mExportImageSequenceSettings->Options.ExportPath.Path.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyPath", "Error: Empty Storyboard Path" );

    if( !mExportImageSequenceSettings->Options.mPatternKeywordLists.IsValidPattern( mExportImageSequenceSettings->Options.Pattern ) )
        return LOCTEXT( "WrongPattern", "Error: Wrong Pattern" );

    if( !mImageSequenceExportErrorMessage.IsEmpty() )
        return FText::FromString( mImageSequenceExportErrorMessage );

    return FText::GetEmpty();
}

FText
SExportStoryboardSettings::GetWarningText() const
{
    return FText::GetEmpty();
}

bool
SExportStoryboardSettings::CanExportStoryboard() const
{
    if( !GetErrorText().IsEmpty() )
        return false;

    if( mExportImageSequenceSettings->Options.ExportPath.Path.IsEmpty() )
        return false;

    return true;
}

FReply
SExportStoryboardSettings::OnExportStoryboard()
{
    FExportStruct image_sequence_struct;
    for( const auto& panel : mPanelItemsList )
    {
        if( !panel->mExport )
            continue;

        image_sequence_struct.Panels.Add( panel->mPanel );
    }

    FExportImageSequenceExporter exporter( mSequencer, &image_sequence_struct, &mExportImageSequenceSettings->Options );
    exporter.Export();

    //---

    mParentWindow.Pin()->RequestDestroyWindow();

    return FReply::Handled();
}

//---
//---
//---

void
ExportStoryboardDialog::OpenExportImageSequenceDialog( const TSharedRef<FTabManager>& TabManager, UMovieSceneSequence* iSequence )
{
    IAssetEditorInstance* assetEditor = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset( iSequence, false );
    IEposSequenceEditorToolkit* eposSequenceEditor = static_cast<IEposSequenceEditorToolkit*>( assetEditor );
    TSharedPtr<ISequencer> sequencer = eposSequenceEditor ? eposSequenceEditor->GetSequencer() : nullptr;
    check( sequencer.IsValid() );

    sequencer->ResetToNewRootSequence( *iSequence );

    //---

    TSharedRef<SWindow> window = SNew( SWindow )
        .Title( LOCTEXT( "ExportStoryboardDialogTitle", "Export Storyboard Settings" ) )
        .HasCloseButton( true )
        .SupportsMaximize( false )
        .SupportsMinimize( false )
        .ClientSize( FVector2D( 1600, 900 ) );

    window->SetContent( SNew( SExportStoryboardSettings, sequencer, iSequence )
                        .ParentWindow( window )
                        );

    GEditor->EditorAddModalWindow( window );

    // To have a none modal dialog for debugging
    //TSharedPtr<SDockTab> OwnerTab = TabManager->GetOwnerTab();
    //TSharedPtr<SWindow> RootWindow = OwnerTab.IsValid() ? OwnerTab->GetParentWindow() : TSharedPtr<SWindow>();
    //if( RootWindow.IsValid() )
    //{
    //    FSlateApplication::Get().AddWindowAsNativeChild( window, RootWindow.ToSharedRef() );
    //}
    //else
    //{
    //    FSlateApplication::Get().AddWindow( window );
    //}
}

#undef LOCTEXT_NAMESPACE
