// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "CinematicBoardTrack/CinematicBoardTrackEditor.h"

#include "AutomatedLevelSequenceCapture.h"
#include "LevelSequence.h"
#include "MovieSceneCaptureModule.h"
#include "MovieSceneTimeHelpers.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"
#include "SequencerSettings.h"
#include "MVVM/Views/ViewUtilities.h"
#include "MVVM/ViewModels/TrackRowModel.h"
#include "MVVM/ViewModels/OutlinerColumns/OutlinerColumnTypes.h"
#include "MVVM/Extensions/ITrackExtension.h"
#include "TrackEditorThumbnail/TrackEditorThumbnailPool.h"
#include "Tracks/MovieSceneSubTrack.h"

#include "Application/ThrottleManager.h"
#include "DragAndDrop/AssetDragDropOp.h"
#include "Editor.h"
#include "FCPXML/FCPXMLMovieSceneTranslator.h"
#include "Factories/Factory.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GameFramework/Actor.h"
#include "LevelEditorViewport.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Styling/AppStyle.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/SBoxPanel.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrackInstance.h"
#include "EposTracksEditorCommands.h"
#include "Styles/EposTracksEditorStyle.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "FCinematicBoardTrackEditor"

/* FCinematicBoardTrackEditor structors
 *****************************************************************************/

FCinematicBoardTrackEditor::FCinematicBoardTrackEditor( TSharedRef<ISequencer> iSequencer )
    : FSubTrackEditor( iSequencer )
{
    mThumbnailPool = MakeShareable( new FTrackEditorThumbnailPool( iSequencer ) );
}


TSharedRef<ISequencerTrackEditor>
FCinematicBoardTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> iSequencer )
{
    return MakeShareable( new FCinematicBoardTrackEditor( iSequencer ) );
}


TWeakObjectPtr<AActor>
FCinematicBoardTrackEditor::GetBoardCamera() const
{
    const UCameraComponent* Camera = GetSequencer()->GetLastEvaluatedCameraCut().Get();
    return Camera ? Camera->GetOwner() : nullptr;
}

void
FCinematicBoardTrackEditor::OnRelease() //override
{
    TSharedPtr<FUICommandList> command_list = GetSequencer().IsValid() ? GetSequencer()->GetCommandBindings() : nullptr;
    if( command_list )
    {
        command_list->UnmapAction( FEposTracksEditorCommands::Get().NewSectionWithBoardAtCurrentFrame );
        command_list->UnmapAction( FEposTracksEditorCommands::Get().NewSectionWithShotAtCurrentFrame );

        command_list->UnmapAction( FEposTracksEditorCommands::Get().ArrangeShotsManually );
        command_list->UnmapAction( FEposTracksEditorCommands::Get().ArrangeShotsOnOneRow );
        command_list->UnmapAction( FEposTracksEditorCommands::Get().ArrangeShotsOnTwoRows );
    }
}

void
FCinematicBoardTrackEditor::BindCommands( TSharedRef<FUICommandList> SequencerCommandBindings ) //override
{
    TSharedPtr<FUICommandList> command_list = GetSequencer().IsValid() ? GetSequencer()->GetCommandBindings() : nullptr;
    if( command_list )
    {
        command_list->MapAction(
            FEposTracksEditorCommands::Get().NewSectionWithBoardAtCurrentFrame,
            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::InsertBoard )
            );
        command_list->MapAction(
            FEposTracksEditorCommands::Get().NewSectionWithShotAtCurrentFrame,
            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::InsertShot )
            );

        command_list->MapAction(
            FEposTracksEditorCommands::Get().ArrangeShotsManually,
            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::SetArrangeSections, EArrangeSections::Manually ),
            FCanExecuteAction::CreateLambda( []() { return true; } ),
            FIsActionChecked::CreateRaw( this, &FCinematicBoardTrackEditor::IsArrangeSections, EArrangeSections::Manually )
        );
        command_list->MapAction(
            FEposTracksEditorCommands::Get().ArrangeShotsOnOneRow,
            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::SetArrangeSections, EArrangeSections::OnOneRow ),
            FCanExecuteAction::CreateLambda( []() { return true; } ),
            FIsActionChecked::CreateRaw( this, &FCinematicBoardTrackEditor::IsArrangeSections, EArrangeSections::OnOneRow )
        );
        command_list->MapAction(
            FEposTracksEditorCommands::Get().ArrangeShotsOnTwoRows,
            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::SetArrangeSections, EArrangeSections::OnTwoRowsShifted ),
            FCanExecuteAction::CreateLambda( []() { return true; } ),
            FIsActionChecked::CreateRaw( this, &FCinematicBoardTrackEditor::IsArrangeSections, EArrangeSections::OnTwoRowsShifted )
        );
    }
}

/* ISequencerTrackEditor interface
 *****************************************************************************/

void
FCinematicBoardTrackEditor::BuildAddTrackMenu( FMenuBuilder& ioMenuBuilder ) //override
{
    ioMenuBuilder.AddMenuEntry(
        FText::Join( FText::FromString( " " ), GetSubTrackName(), LOCTEXT( "TrackText", "Track" ) ),
        GetSubTrackToolTip(),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), GetSubTrackBrushName() ),
        FUIAction(
            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::HandleAddSubTrackMenuEntryExecute ),
            FCanExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::HandleAddSubTrackMenuEntryCanExecute ),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateRaw( this, &FCinematicBoardTrackEditor::HandleAddCinematicBoardTrackMenuEntryIsVisible )
        )
    );
}

bool
FCinematicBoardTrackEditor::HandleAddSubTrackMenuEntryCanExecute() const
{
    UMovieScene* focusedMovieScene = GetFocusedMovieScene();

    return ( ( focusedMovieScene != nullptr ) && ( focusedMovieScene->FindTrack<UMovieSceneCinematicBoardTrack>() == nullptr ) );
}


void
FCinematicBoardTrackEditor::HandleAddSubTrackMenuEntryExecute()
{
    UMovieSceneCinematicBoardTrack* boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( GetSequencer().Get() );
    if( boardTrack )
    {
        if( GetSequencer().IsValid() )
        {
            // Board Tracks can't be placed in folders, they're only allowed in the root.
            GetSequencer()->OnAddTrack( boardTrack, FGuid() );
        }
    }
}

bool
FCinematicBoardTrackEditor::HandleAddCinematicBoardTrackMenuEntryIsVisible()
{
    UMovieSceneSequence* FocusedSequence = GetSequencer()->GetFocusedMovieSceneSequence();

    return ( ( FocusedSequence != nullptr ) && ( FocusedSequence->IsA<UBoardSequence>() ) );
}

//---

TSharedPtr<SWidget>
FCinematicBoardTrackEditor::BuildOutlinerColumnWidget( const FBuildColumnWidgetParams& Params, const FName& ColumnName ) //override
{
    using namespace UE::Sequencer;

    if( ColumnName == FCommonOutlinerNames::Add )
    {
        return UE::Sequencer::MakeAddButton(
            LOCTEXT( "CreateBoardShotText", "Shot/Board" ),
            FOnGetContent::CreateSP( this, &FCinematicBoardTrackEditor::HandleAddSubSequenceComboButtonGetMenuContent, Params.TrackModel.AsWeak() ),
            Params.ViewModel );
    }

    if( !Params.ViewModel->IsA<FTrackRowModel>() )
    {
        bool bAddCameraLock = false;
        if( ColumnName == FCommonOutlinerNames::Nav )
        {
            bAddCameraLock = true;
        }
        else if( ColumnName == FCommonOutlinerNames::KeyFrame )
        {
            // Add the camera lock button to the keyframe column if Nav is disabled
            bAddCameraLock = Params.TreeViewRow->IsColumnVisible( FCommonOutlinerNames::Nav ) == false;
        }
        else if( ColumnName == FCommonOutlinerNames::Edit )
        {
            // Add the camera lock button to the edit column if both Nav and KeyFrame are disabled
            bAddCameraLock = Params.TreeViewRow->IsColumnVisible( FCommonOutlinerNames::Nav ) == false &&
                Params.TreeViewRow->IsColumnVisible( FCommonOutlinerNames::KeyFrame ) == false;
        }

        if( bAddCameraLock )
        {
            TSharedRef<SWidget> Button = SNew( SCheckBox )
                .Style( FAppStyle::Get(), "Sequencer.Outliner.ToggleButton" )
                .Type( ESlateCheckBoxType::ToggleButton )
                .IsFocusable( false )
                .IsChecked( this, &FCinematicBoardTrackEditor::AreBoardsLocked )
                .OnCheckStateChanged( this, &FCinematicBoardTrackEditor::OnLockBoardsClicked )
                .ToolTipText( this, &FCinematicBoardTrackEditor::GetLockBoardsToolTip )
                [
                    SNew( SImage )
                        .Image( FAppStyle::GetBrush( "Sequencer.Outliner.CameraLock" ) )
                ];

            if( ColumnName == FCommonOutlinerNames::Edit )
            {
                // Needs to be left aligned in the edit column because this column slot is set to fill
                return SNew( SBox )
                    .HAlign( HAlign_Left )
                    .Padding( 4.f, 0.f )
                    [
                        Button
                    ];
            }
            else
            {
                return Button;
            }
        }
    }

    return FMovieSceneTrackEditor::BuildOutlinerColumnWidget( Params, ColumnName );
}

TSharedRef<SWidget>
FCinematicBoardTrackEditor::HandleAddSubSequenceComboButtonGetMenuContent( UE::Sequencer::TWeakViewModelPtr<UE::Sequencer::ITrackExtension> WeakTrackModel )
{
    FMenuBuilder menuBuilder( true, GetSequencer()->GetCommandBindings() );

    menuBuilder.AddMenuEntry( FEposTracksEditorCommands::Get().NewSectionWithShotAtCurrentFrame );
    menuBuilder.AddMenuEntry( FEposTracksEditorCommands::Get().NewSectionWithBoardAtCurrentFrame );

    return menuBuilder.MakeWidget();
}

//---

TSharedRef<ISequencerSection>
FCinematicBoardTrackEditor::MakeSectionInterface( UMovieSceneSection& iSectionObject, UMovieSceneTrack& ioTrack, FGuid iObjectBinding ) //override
{
    check( SupportsType( iSectionObject.GetOuter()->GetClass() ) );

    UMovieSceneCinematicBoardSection& sectionObjectImpl = *CastChecked<UMovieSceneCinematicBoardSection>( &iSectionObject );
    return MakeShareable( new FCinematicBoardSection( GetSequencer(), sectionObjectImpl, SharedThis( this ), mThumbnailPool ) );
}

//---

FText
FCinematicBoardTrackEditor::GetSubTrackName() const
{
    return LOCTEXT( "BoardTrackName", "Board" );
}

FText
FCinematicBoardTrackEditor::GetSubTrackToolTip() const
{
    return LOCTEXT( "BoardTrackToolTip", "A cinematic board track." );
}

FName
FCinematicBoardTrackEditor::GetSubTrackBrushName() const
{
    return TEXT( "Sequencer.Tracks.CinematicBoard" );
}

FString
FCinematicBoardTrackEditor::GetSubSectionDisplayName( const UMovieSceneSubSection* Section ) const
{
    return Cast<UMovieSceneCinematicBoardSection>( Section )->GetBoardDisplayName();
}

FString
FCinematicBoardTrackEditor::GetDefaultSubsequenceName() const
{
    checkNoEntry();
    return FString();

    // Don't care about this as in subtrackeditor, it's used in insertsection()/duplicatesection(), which are override here
    // Or maybe use naming convention ?

    //const UMovieSceneToolsProjectSettings* ProjectSettings = GetDefault<UMovieSceneToolsProjectSettings>();
    //return ProjectSettings->ShotPrefix;
}

FString
FCinematicBoardTrackEditor::GetDefaultSubsequenceDirectory() const
{
    checkNoEntry();
    return FString();

    // Don't care about this as in subtrackeditor, it's used in insertsection()/duplicatesection(), which are override here
    // Or maybe use naming convention ?

    //const UMovieSceneToolsProjectSettings* ProjectSettings = GetDefault<UMovieSceneToolsProjectSettings>();
    //return ProjectSettings->ShotDirectory;
}

TSubclassOf<UMovieSceneSubTrack>
FCinematicBoardTrackEditor::GetSubTrackClass() const
{
    return UMovieSceneCinematicBoardTrack::StaticClass();
}

//---

void
FCinematicBoardTrackEditor::GetSupportedSequenceClassPaths( TArray<FTopLevelAssetPath>& oClassPaths ) const
{
    checkNoEntry();

    // In FSubTrackEditor, it's used in the [Add Sequence] popup, which is not used here for board track
    // So, must be tested first if this function is really needed

    //oClassPaths.Add( FTopLevelAssetPath( TEXT( "/Script/BoardSequence" ), TEXT( "BoardSequence" ) ) );
    //oClassPaths.Add( FTopLevelAssetPath( TEXT( "/Script/ShotSequence" ), TEXT( "ShotSequence" ) ) );
}

bool
FCinematicBoardTrackEditor::CanHandleAssetAdded( UMovieSceneSequence* iSequence ) const
{
    // Only allow sequences with a camera cut track to be dropped as a shot. Otherwise, it'll be dropped as a subsequence.
    //return iSequence->GetMovieScene()->GetCameraCutTrack() != nullptr;

    // Handle asset added only if there is no already existing cinematic board track
    // Because in FSubTrackEditor::HandleAssetAdded(), a new track is always created (also bug with level sequence)
    // And if a cinematic board track already exists, just drop on this track
    return !BoardSequenceTools::FindCinematicBoardTrack( GetSequencer().Get() );
}

UMovieSceneSubTrack*
FCinematicBoardTrackEditor::FindOrCreateSubTrack( UMovieScene* MovieScene, UMovieSceneTrack* Track ) const
{
    UMovieSceneSubTrack* SubTrack = Cast<UMovieSceneSubTrack>( Track );
    if( !SubTrack )
    {
        SubTrack = Cast<UMovieSceneSubTrack>( MovieScene->AddTrack( GetSubTrackClass() ) );
    }
    return SubTrack;
}
//
//bool
//FCinematicBoardTrackEditor::HandleAssetAdded( UObject* iAsset, const FGuid& iTargetObjectGuid ) //override
//{
//    UMovieSceneSequence* sequence = Cast<UMovieSceneSequence>( iAsset );
//
//    if( sequence == nullptr )
//    {
//        return false;
//    }
//
//    if( !( ( sequence->GetClass()->GetName() == TEXT( "BoardSequence" ) ) || ( sequence->GetClass()->GetName() == TEXT( "ShotSequence" ) ) ) )
//    //if( !SupportsSequence( sequence ) ) // ??????????????????????????????????????????????
//    {
//        return false;
//    }
//
//    if( !CanHandleAssetAdded( sequence ) )
//    {
//        return false;
//    }
//
//    if( sequence->GetMovieScene()->GetPlaybackRange().IsEmpty() )
//    {
//        FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequenceDuration", "Invalid level sequence {0}. The sequence has no duration." ), sequence->GetDisplayName() ) );
//        info.bUseLargeFont = false;
//        FSlateNotificationManager::Get().AddNotification( info );
//        return false;
//    }
//
//    if( CanAddSubSequence( *sequence ) )
//    {
//        const FScopedTransaction transaction( FText::Join( FText::FromString( " " ), LOCTEXT( "AddText", "Add" ), GetSubTrackName(), LOCTEXT( "TrackText", "Track" ) ) );
//
//        int32 rowIndex = INDEX_NONE;
//        UMovieSceneTrack* track = nullptr;
//        AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FCinematicBoardTrackEditor::HandleSequenceAdded, sequence, track, rowIndex ) );
//
//        return true;
//    }
//
//    FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequence", "Invalid level sequence {0}. There could be a circular dependency." ), sequence->GetDisplayName() ) );
//    info.bUseLargeFont = false;
//    FSlateNotificationManager::Get().AddNotification( info );
//
//    return false;
//}
//
//FKeyPropertyResult
//FCinematicBoardTrackEditor::HandleSequenceAdded( FFrameNumber iKeyTime, UMovieSceneSequence* iSequence, UMovieSceneTrack* iTrack, int32 iRowIndex )
//{
//    FKeyPropertyResult keyPropertyResult;
//
//    UMovieScene* movieScene = GetFocusedMovieScene();
//
//    UMovieSceneSubTrack* subTrack = FindOrCreateSubTrack( movieScene, iTrack );
//    //auto boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( GetSequencer().Get() );
//
//    const FFrameRate tickResolution = iSequence->GetMovieScene()->GetTickResolution();
//    const FQualifiedFrameTime innerDuration = FQualifiedFrameTime(
//        UE::MovieScene::DiscreteSize( iSequence->GetMovieScene()->GetPlaybackRange() ),
//        tickResolution );
//
//    const FFrameRate outerFrameRate = subTrack->GetTypedOuter<UMovieScene>()->GetTickResolution();
//    const int32      outerDuration = innerDuration.ConvertTo( outerFrameRate ).FrameNumber.Value;
//
//    UMovieSceneSubSection* newSection = subTrack->AddSequenceOnRow( iSequence, iKeyTime, outerDuration, iRowIndex );
//    keyPropertyResult.bTrackModified = true;
//    keyPropertyResult.SectionsCreated.Add( newSection );
//
//    BoardSequenceTools::UpdateViewRange( GetSequencer().Get(), newSection->GetTrueRange() );
//    GetSequencer()->EmptySelection();
//    GetSequencer()->SelectSection( newSection );
//    GetSequencer()->ThrobSectionSelection();
//
//    if( tickResolution != outerFrameRate )
//    {
//        FNotificationInfo info( FText::Format( LOCTEXT( "TickResolutionMismatch", "The parent sequence has a different tick resolution {0} than the newly added sequence {1}" ), outerFrameRate.ToPrettyText(), tickResolution.ToPrettyText() ) );
//        info.bUseLargeFont = false;
//        FSlateNotificationManager::Get().AddNotification( info );
//    }
//
//    return keyPropertyResult;
//}


bool
FCinematicBoardTrackEditor::SupportsSequence( UMovieSceneSequence* iSequence ) const //override
{
    UMovieSceneSequence* focusedSequence = GetSequencer()->GetFocusedMovieSceneSequence();
    ETrackSupport trackSupported = focusedSequence ? focusedSequence->IsTrackSupported( UMovieSceneCinematicBoardTrack::StaticClass() ) : ETrackSupport::NotSupported;

    // This means that the [Add Track] button is pressed
    if( iSequence == focusedSequence )
        return trackSupported == ETrackSupport::Supported || focusedSequence->IsA( ULevelSequence::StaticClass() );

    if( !iSequence )
        return false;

    // Here it should be: iSequence is dropped on focusedSequence
    return ( trackSupported == ETrackSupport::Supported ) && ( ( iSequence->GetClass()->GetName() == TEXT( "BoardSequence" ) ) || ( iSequence->GetClass()->GetName() == TEXT( "ShotSequence" ) ) );
}

//
//bool
//FCinematicBoardTrackEditor::SupportsType( TSubclassOf<UMovieSceneTrack> iType ) const //override
//{
//    return ( iType == GetSubTrackClass() );
//}


void
FCinematicBoardTrackEditor::Tick( float iDeltaTime ) //override
{
    TSharedPtr<ISequencer> sequencerPin = GetSequencer();
    if( !sequencerPin.IsValid() )
    {
        return;
    }

    EMovieScenePlayerStatus::Type playbackState = sequencerPin->GetPlaybackStatus();

    if( FSlateThrottleManager::Get().IsAllowingExpensiveTasks() && playbackState != EMovieScenePlayerStatus::Playing && playbackState != EMovieScenePlayerStatus::Scrubbing )
    {
        sequencerPin->EnterSilentMode();

        FFrameTime savedTime = sequencerPin->GetGlobalTime().Time;

        if( iDeltaTime > 0.f && mThumbnailPool->DrawThumbnails() )
        {
            sequencerPin->SetGlobalTime( savedTime );
        }

        sequencerPin->ExitSilentMode();
    }
}


void
FCinematicBoardTrackEditor::BuildTrackContextMenu( FMenuBuilder& ioMenuBuilder, UMovieSceneTrack* iTrack ) //override
{
    ioMenuBuilder.AddSubMenu(
        LOCTEXT( "ArrangeSections", "Arrange Sections" ),
        LOCTEXT( "ArrangeSectionsTooltip", "Arrange sections." ),
        FNewMenuDelegate::CreateLambda( [this]( FMenuBuilder& ioSubMenuBuilder )
                                        {
                                            ioSubMenuBuilder.AddMenuEntry( FEposTracksEditorCommands::Get().ArrangeShotsManually );
                                            ioSubMenuBuilder.AddMenuEntry( FEposTracksEditorCommands::Get().ArrangeShotsOnOneRow );
                                            ioSubMenuBuilder.AddMenuEntry( FEposTracksEditorCommands::Get().ArrangeShotsOnTwoRows );
                                        } )
    );

    ioMenuBuilder.AddSeparator();

    //ioMenuBuilder.BeginSection( "Import/Export", NSLOCTEXT( "Sequencer", "ImportExportMenuSectionName", "Import/Export" ) );

    //ioMenuBuilder.AddMenuEntry(
    //    NSLOCTEXT( "Sequencer", "ImportEDL", "Import EDL..." ),
    //    NSLOCTEXT( "Sequencer", "ImportEDLTooltip", "Import Edit Decision List (EDL) for non-linear editors." ),
    //    FSlateIcon(),
    //    FUIAction(
    //        FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::ImportEDL ) ) );

    //ioMenuBuilder.AddMenuEntry(
    //    NSLOCTEXT( "Sequencer", "ExportEDL", "Export EDL..." ),
    //    NSLOCTEXT( "Sequencer", "ExportEDLTooltip", "Export Edit Decision List (EDL) for non-linear editors." ),
    //    FSlateIcon(),
    //    FUIAction(
    //        FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::ExportEDL ) ) );

    //ioMenuBuilder.AddMenuEntry(
    //    NSLOCTEXT( "Sequencer", "ImportFCPXML", "Import Final Cut Pro 7 XML..." ),
    //    NSLOCTEXT( "Sequencer", "ImportFCPXMLTooltip", "Import Final Cut Pro 7 XML file for non-linear editors." ),
    //    FSlateIcon(),
    //    FUIAction(
    //        FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::ImportFCPXML ) ) );

    //ioMenuBuilder.AddMenuEntry(
    //    NSLOCTEXT( "Sequencer", "ExportFCPXML", "Export Final Cut Pro 7 XML..." ),
    //    NSLOCTEXT( "Sequencer", "ExportFCPXMLTooltip", "Export Final Cut Pro 7 XML file for non-linear editors." ),
    //    FSlateIcon(),
    //    FUIAction(
    //        FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::ExportFCPXML ) ) );

    //ioMenuBuilder.EndSection();
}

void
FCinematicBoardTrackEditor::SetArrangeSections( EArrangeSections iArrangeSections )
{
    auto board_track = BoardSequenceTools::FindCinematicBoardTrack( GetSequencer().Get() );
    if( !board_track )
        return;

    board_track->SetArrangeSections( iArrangeSections );

    GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
}

bool
FCinematicBoardTrackEditor::IsArrangeSections( EArrangeSections iArrangeSections )
{
    auto board_track = BoardSequenceTools::FindCinematicBoardTrack( GetSequencer().Get() );
    if( !board_track )
        return false;

    return board_track->GetArrangeSections() == iArrangeSections;
}

//---

const FSlateBrush*
FCinematicBoardTrackEditor::GetIconBrush() const //override
{
    return FEposTracksEditorStyle::Get().GetBrush( GetSubTrackBrushName() );
}
//
//bool
//FCinematicBoardTrackEditor::OnAllowDrop( const FDragDropEvent& iDragDropEvent, FSequencerDragDropParams& DragDropParams ) //override
//{
//    if( !DragDropParams.Track.IsValid() )
//    {
//        return false;
//    }
//
//    if( !DragDropParams.Track.Get()->IsA( GetSubTrackClass() ) )
//    {
//        return false;
//    }
//
//    TSharedPtr<FDragDropOperation> operation = iDragDropEvent.GetOperation();
//
//    if( !operation.IsValid() || !operation->IsOfType<FAssetDragDropOp>() )
//    {
//        return false;
//    }
//
//    TSharedPtr<ISequencer> sequencerPtr = GetSequencer();
//    if( !sequencerPtr )
//    {
//        return false;
//    }
//
//    UMovieSceneSequence* focusedSequence = sequencerPtr->GetFocusedMovieSceneSequence();
//    if( !focusedSequence )
//    {
//        return false;
//    }
//
//    TSharedPtr<FAssetDragDropOp> dragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( operation );
//
//    TOptional<FFrameNumber> LongestLengthInFrames;
//    for( const FAssetData& assetData : dragDropOp->GetAssets() )
//    {
//        if( !MovieSceneToolHelpers::IsValidAsset( focusedSequence, assetData ) )
//        {
//            continue;
//        }
//
//        UMovieSceneSequence* sequence = Cast<UMovieSceneSequence>( assetData.GetAsset() );
//        if( sequence && CanAddSubSequence( *sequence ) )
//        {
//            FFrameRate TickResolution = sequencerPtr->GetFocusedTickResolution();
//
//            const FQualifiedFrameTime InnerDuration = FQualifiedFrameTime(
//                UE::MovieScene::DiscreteSize( sequence->GetMovieScene()->GetPlaybackRange() ),
//                sequence->GetMovieScene()->GetTickResolution() );
//
//            FFrameNumber LengthInFrames = InnerDuration.ConvertTo( TickResolution ).FrameNumber;
//
//            // Keep track of the longest sub-sequence asset we're trying to drop onto it for preview display purposes.
//            LongestLengthInFrames = FMath::Max( LongestLengthInFrames.Get( FFrameNumber( 0 ) ), LengthInFrames );
//        }
//    }
//
//    if( LongestLengthInFrames.IsSet() )
//    {
//        DragDropParams.FrameRange = TRange<FFrameNumber>( DragDropParams.FrameNumber, DragDropParams.FrameNumber + LongestLengthInFrames.GetValue() );
//        return true;
//    }
//
//    return false;
//}
//
//FReply
//FCinematicBoardTrackEditor::OnDrop( const FDragDropEvent& iDragDropEvent, const FSequencerDragDropParams& DragDropParams ) //override
//{
//    if( !DragDropParams.Track.IsValid() )
//    {
//        return FReply::Unhandled();
//    }
//
//    if( !DragDropParams.Track.Get()->IsA( GetSubTrackClass() ) )
//    {
//        return FReply::Unhandled();
//    }
//
//    TSharedPtr<FDragDropOperation> operation = iDragDropEvent.GetOperation();
//
//    if( !operation.IsValid() || !operation->IsOfType<FAssetDragDropOp>() )
//    {
//        return FReply::Unhandled();
//    }
//
//    TSharedPtr<ISequencer> sequencerPtr = GetSequencer();
//    if( !sequencerPtr )
//    {
//        return FReply::Unhandled();
//    }
//
//    UMovieSceneSequence* focusedSequence = sequencerPtr->GetFocusedMovieSceneSequence();
//    if( !focusedSequence )
//    {
//        return FReply::Unhandled();
//    }
//
//    const FScopedTransaction Transaction( LOCTEXT( "DropAssets", "Drop Assets" ) );
//
//    TSharedPtr<FAssetDragDropOp> dragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( operation );
//
//    FMovieSceneTrackEditor::BeginKeying( DragDropParams.FrameNumber );
//
//    bool anyDropped = false;
//    for( const FAssetData& assetData : dragDropOp->GetAssets() )
//    {
//        if( !MovieSceneToolHelpers::IsValidAsset( focusedSequence, assetData ) )
//        {
//            continue;
//        }
//
//        UMovieSceneSequence* sequence = Cast<UMovieSceneSequence>( assetData.GetAsset() );
//        if( CanAddSubSequence( *sequence ) )
//        {
//            AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FCinematicBoardTrackEditor::HandleSequenceAdded, sequence, DragDropParams.Track.Get(), DragDropParams.RowIndex ) );
//            //AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FCinematicBoardTrackEditor::AddKeyInternal, sequence, DragDropParams.Track.Get(), DragDropParams.RowIndex, TOptional<FFrameNumber>( DragDropParams.FrameNumber ) ) );
//
//            anyDropped = true;
//        }
//    }
//
//    FMovieSceneTrackEditor::EndKeying();
//
//    return anyDropped ? FReply::Handled() : FReply::Unhandled();
//}

FReply
FCinematicBoardTrackEditor::OnDrop( const FDragDropEvent& iDragDropEvent, const FSequencerDragDropParams& DragDropParams ) //override
{
    TSharedPtr<FDragDropOperation> operation = iDragDropEvent.GetOperation();
    if( !operation.IsValid() || !operation->IsOfType<FAssetDragDropOp>() )
        return FReply::Unhandled();

    TSharedPtr<FAssetDragDropOp> dragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( operation );

    for( const FAssetData& assetData : dragDropOp->GetAssets() )
    {
        UMovieSceneSequence* sequence = Cast<UMovieSceneSequence>( assetData.GetAsset() );
        if( !SupportsSequence( sequence ) )
            return FReply::Unhandled();
    }

    //---

    FReply reply = FSubTrackEditor::OnDrop( iDragDropEvent, DragDropParams );

    if( reply.IsEventHandled() )
        BoardSequenceTools::UpdateViewRange( GetSequencer().Get(), DragDropParams.FrameRange );

    return reply;
}

//---

void
FCinematicBoardTrackEditor::InsertBoard()
{
    CinematicBoardTrackTools::InsertBoard( GetSequencer().Get(), GetSequencer()->GetLocalTime().Time.FrameNumber );
}

void
FCinematicBoardTrackEditor::InsertShot()
{
    CinematicBoardTrackTools::InsertShot( GetSequencer().Get(), GetSequencer()->GetLocalTime().Time.FrameNumber );
}

/* FCinematicBoardTrackEditor callbacks
 *****************************************************************************/

//
//FKeyPropertyResult
//FCinematicBoardTrackEditor::AddKeyInternal( FFrameNumber iKeyTime, UMovieSceneSequence* iMovieSceneSequence, UMovieSceneTrack* iTrack, int32 iRowIndex, TOptional<FFrameNumber> iDroppedFrame )
//{
//    FKeyPropertyResult keyPropertyResult;
//
//    if( !( ( iMovieSceneSequence->GetClass()->GetName() == TEXT( "BoardSequence" ) ) || ( iMovieSceneSequence->GetClass()->GetName() == TEXT( "ShotSequence" ) ) ) )
//        return keyPropertyResult;
//
//    if( iMovieSceneSequence->GetMovieScene()->GetPlaybackRange().IsEmpty() )
//    {
//        FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequenceDuration", "Invalid level sequence {0}. The sequence has no duration." ), iMovieSceneSequence->GetDisplayName() ) );
//        info.bUseLargeFont = false;
//        FSlateNotificationManager::Get().AddNotification( info );
//        return keyPropertyResult;
//    }
//
//    if( CanAddSubSequence( *iMovieSceneSequence ) )
//    {
//        UMovieScene* movieScene = GetFocusedMovieScene();
//
//        //UMovieSceneSubTrack* subTrack = FindOrCreateSubTrack( movieScene, iTrack );
//        UMovieSceneCinematicBoardTrack* subTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( GetSequencer().Get() );
//
//        const FFrameRate tickResolution = iMovieSceneSequence->GetMovieScene()->GetTickResolution();
//        const FQualifiedFrameTime innerDuration = FQualifiedFrameTime(
//            UE::MovieScene::DiscreteSize( iMovieSceneSequence->GetMovieScene()->GetPlaybackRange() ),
//            tickResolution );
//
//        const FFrameRate outerFrameRate = subTrack->GetTypedOuter<UMovieScene>()->GetTickResolution();
//        const int32      outerDuration = innerDuration.ConvertTo( outerFrameRate ).FrameNumber.Value;
//
//        UMovieSceneSubSection* newSection = subTrack->AddSequenceOnRow( iMovieSceneSequence, iDroppedFrame.IsSet() ? iDroppedFrame.GetValue() : iKeyTime, outerDuration, iRowIndex );
//        keyPropertyResult.bTrackModified = true;
//        keyPropertyResult.SectionsCreated.Add( newSection );
//
//        BoardSequenceTools::UpdateViewRange( GetSequencer().Get(), newSection->GetTrueRange() );
//        GetSequencer()->EmptySelection();
//        GetSequencer()->SelectSection( newSection );
//        GetSequencer()->ThrobSectionSelection();
//
//        if( tickResolution != outerFrameRate )
//        {
//            FNotificationInfo info( FText::Format( LOCTEXT( "TickResolutionMismatch", "The parent sequence has a different tick resolution {0} than the newly added sequence {1}" ), outerFrameRate.ToPrettyText(), tickResolution.ToPrettyText() ) );
//            info.bUseLargeFont = false;
//            FSlateNotificationManager::Get().AddNotification( info );
//        }
//
//        return keyPropertyResult;
//    }
//
//    FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequence", "Invalid level sequence {0}. There could be a circular dependency." ), iMovieSceneSequence->GetDisplayName() ) );
//    info.bUseLargeFont = false;
//    FSlateNotificationManager::Get().AddNotification( info );
//
//    return keyPropertyResult;
//}

//---
//
//bool
//FCinematicBoardTrackEditor::CanAddSubSequence( const UMovieSceneSequence& iSequence ) const
//{
//    // prevent adding ourselves and ensure we have a valid movie scene
//    UMovieSceneSequence* focusedSequence = GetSequencer()->GetFocusedMovieSceneSequence();
//
//    if( ( focusedSequence == nullptr ) || ( focusedSequence == &iSequence ) || ( focusedSequence->GetMovieScene() == nullptr ) )
//    {
//        return false;
//    }
//
//    // ensure that the other sequence has a valid movie scene
//    UMovieScene* sequenceMovieScene = iSequence.GetMovieScene();
//
//    if( sequenceMovieScene == nullptr )
//    {
//        return false;
//    }
//
//    // make sure we are not contained in the other sequence (circular dependency)
//    // @todo sequencer: this check is not sufficient (does not prevent circular dependencies of 2+ levels)
//    UMovieSceneSubTrack* sequenceSubTrack = sequenceMovieScene->FindTrack<UMovieSceneSubTrack>();
//    if( sequenceSubTrack && sequenceSubTrack->ContainsSequence( *focusedSequence, true ) )
//    {
//        return false;
//    }
//
//    UMovieSceneCinematicBoardTrack* boardTrack = sequenceMovieScene->FindTrack<UMovieSceneCinematicBoardTrack>();
//    if( boardTrack && boardTrack->ContainsSequence( *focusedSequence, true ) )
//    {
//        return false;
//    }
//
//    return true;
//}

//---

ECheckBoxState
FCinematicBoardTrackEditor::AreBoardsLocked() const
{
    if( GetSequencer()->IsPerspectiveViewportCameraCutEnabled() )
    {
        return ECheckBoxState::Checked;
    }
    else
    {
        return ECheckBoxState::Unchecked;
    }
}

void
FCinematicBoardTrackEditor::OnLockBoardsClicked( ECheckBoxState iCheckBoxState )
{
    TSharedPtr<ISequencer> SequencerPtr = GetSequencer();

    const bool bEnableCameraCuts = ( iCheckBoxState == ECheckBoxState::Checked );
    SequencerPtr->SetPerspectiveViewportCameraCutEnabled( bEnableCameraCuts );

    bool bNeedsRestoreViewport = true;
    if( const USequencerSettings* SequencerSettings = SequencerPtr->GetSequencerSettings() )
    {
        bNeedsRestoreViewport = SequencerSettings->GetRestoreOriginalViewportOnCameraCutUnlock();
    }

    UMovieSceneEntitySystemLinker* Linker = SequencerPtr->GetEvaluationTemplate().GetEntitySystemLinker();
    UMovieSceneSingleCameraCutTrackInstance::ToggleCameraCutLock( Linker, bEnableCameraCuts, bNeedsRestoreViewport );

    SequencerPtr->ForceEvaluate();
}

FText
FCinematicBoardTrackEditor::GetLockBoardsToolTip() const
{
    return AreBoardsLocked() == ECheckBoxState::Checked ?
        LOCTEXT( "UnlockBoards", "Unlock Viewport from Boards" ) :
        LOCTEXT( "LockBoards", "Lock Viewport to Boards" );
}


#undef LOCTEXT_NAMESPACE
