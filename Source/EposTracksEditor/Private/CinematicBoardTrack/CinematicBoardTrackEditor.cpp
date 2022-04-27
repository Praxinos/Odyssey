// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "CinematicBoardTrack/CinematicBoardTrackEditor.h"

#include "Misc/Paths.h"
#include "Widgets/SBoxPanel.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameFramework/Actor.h"
#include "Factories/Factory.h"
#include "Tracks/MovieSceneSubTrack.h"
#include "Modules/ModuleManager.h"
#include "Application/ThrottleManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"
#include "LevelEditorViewport.h"
#include "CommonMovieSceneTools.h"
#include "MovieSceneToolHelpers.h"
#include "FCPXML/FCPXMLMovieSceneTranslator.h"
#include "SequencerUtilities.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"
#include "TrackEditorThumbnail/TrackEditorThumbnailPool.h"
#include "MovieSceneToolsProjectSettings.h"
#include "Editor.h"
#include "DragAndDrop/AssetDragDropOp.h"
#include "MovieSceneTimeHelpers.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposTracksEditorCommands.h"
#include "Styles/EposTracksEditorStyle.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "FCinematicBoardTrackEditor"

/* FCinematicBoardTrackEditor structors
 *****************************************************************************/

FCinematicBoardTrackEditor::FCinematicBoardTrackEditor( TSharedRef<ISequencer> iSequencer )
    : FMovieSceneTrackEditor( iSequencer )
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
    return mBoardCamera;
}


void
FCinematicBoardTrackEditor::OnInitialize() //override
{
    mOnCameraCutHandle = GetSequencer()->OnCameraCut().AddSP( this, &FCinematicBoardTrackEditor::OnUpdateCameraCut );

    //---

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

void
FCinematicBoardTrackEditor::OnUpdateCameraCut( UObject* iCameraObject, bool iJumpCut )
{
    // Keep track of the camera when it switches so that the thumbnail can be drawn with the correct camera
    mBoardCamera = Cast<AActor>( iCameraObject );
}


void
FCinematicBoardTrackEditor::OnRelease() //override
{
    if( mOnCameraCutHandle.IsValid() && GetSequencer().IsValid() )
    {
        GetSequencer()->OnCameraCut().Remove( mOnCameraCutHandle );
    }

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


/* ISequencerTrackEditor interface
 *****************************************************************************/

void
FCinematicBoardTrackEditor::BuildAddTrackMenu( FMenuBuilder& ioMenuBuilder ) //override
{
    ioMenuBuilder.AddMenuEntry(
        LOCTEXT( "AddCinematicBoardTrack", "Board Track" ),
        LOCTEXT( "AddCinematicBoardTooltip", "Adds a board track." ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "Sequencer.Tracks.CinematicBoard" ),
        FUIAction(
            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::HandleAddCinematicBoardTrackMenuEntryExecute ),
            FCanExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::HandleAddCinematicBoardTrackMenuEntryCanExecute )
        )
    );
}

bool
FCinematicBoardTrackEditor::HandleAddCinematicBoardTrackMenuEntryCanExecute() const
{
    UMovieScene* focusedMovieScene = GetFocusedMovieScene();

    return ( ( focusedMovieScene != nullptr ) && ( focusedMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>() == nullptr ) );
}


void
FCinematicBoardTrackEditor::HandleAddCinematicBoardTrackMenuEntryExecute()
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

//---

TSharedPtr<SWidget>
FCinematicBoardTrackEditor::BuildOutlinerEditWidget( const FGuid& iObjectBinding, UMovieSceneTrack* iTrack, const FBuildEditWidgetParams& iParams ) //override
{
    // Create a container edit box
    return SNew( SHorizontalBox )

        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign( VAlign_Center )
        [
            FSequencerUtilities::MakeAddButton( LOCTEXT( "CreateBoardShotText", "Shot/Board" ), FOnGetContent::CreateSP( this, &FCinematicBoardTrackEditor::HandleAddBoardComboButtonGetMenuContent ), iParams.NodeIsHovered, GetSequencer() )
        ]

        // Add the camera check box
        + SHorizontalBox::Slot()
        .VAlign( VAlign_Center )
        .HAlign( HAlign_Right )
        .AutoWidth()
        .Padding( 4, 0, 0, 0 )
        [
            SNew( SCheckBox )
            .Style( &FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>( "ToggleButtonCheckBoxAlt" ) )
            .Type( ESlateCheckBoxType::CheckBox )
            .Padding( FMargin( 0.f ) )
            .IsFocusable( false )
            .IsChecked( this, &FCinematicBoardTrackEditor::AreBoardsLocked )
            .OnCheckStateChanged( this, &FCinematicBoardTrackEditor::OnLockBoardsClicked )
            .ToolTipText( this, &FCinematicBoardTrackEditor::GetLockBoardsToolTip )
            .CheckedImage( FAppStyle::Get().GetBrush( "Sequencer.LockCamera" ) )
            .CheckedHoveredImage( FAppStyle::Get().GetBrush( "Sequencer.LockCamera" ) )
            .CheckedPressedImage( FAppStyle::Get().GetBrush( "Sequencer.LockCamera" ) )
            .UncheckedImage( FAppStyle::Get().GetBrush( "Sequencer.UnlockCamera" ) )
            .UncheckedHoveredImage( FAppStyle::Get().GetBrush( "Sequencer.UnlockCamera" ) )
            .UncheckedPressedImage( FAppStyle::Get().GetBrush( "Sequencer.UnlockCamera" ) )
        ];
}

TSharedRef<SWidget>
FCinematicBoardTrackEditor::HandleAddBoardComboButtonGetMenuContent()
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


bool
FCinematicBoardTrackEditor::HandleAssetAdded( UObject* iAsset, const FGuid& iTargetObjectGuid ) //override
{
    UMovieSceneSequence* sequence = Cast<UMovieSceneSequence>( iAsset );

    if( sequence == nullptr )
    {
        return false;
    }

    if( !( ( sequence->GetClass()->GetName() == TEXT( "BoardSequence" ) ) || ( sequence->GetClass()->GetName() == TEXT( "ShotSequence" ) ) ) )
    //if( !SupportsSequence( sequence ) )
    {
        return false;
    }

    // Only allow sequences with a camera cut track to be dropped as a shot. Otherwise, it'll be dropped as a subsequence.s
    if( !sequence->GetMovieScene()->GetCameraCutTrack() )
    {
        return false;
    }

    if( sequence->GetMovieScene()->GetPlaybackRange().IsEmpty() )
    {
        FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequenceDuration", "Invalid level sequence {0}. The sequence has no duration." ), sequence->GetDisplayName() ) );
        info.bUseLargeFont = false;
        FSlateNotificationManager::Get().AddNotification( info );
        return false;
    }

    if( CanAddSubSequence( *sequence ) )
    {
        const FScopedTransaction transaction( LOCTEXT( "AddBoard_Transaction", "Add Board" ) );

        int32 rowIndex = INDEX_NONE;
        AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FCinematicBoardTrackEditor::HandleSequenceAdded, sequence, rowIndex ) );

        return true;
    }

    FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequence", "Invalid level sequence {0}. There could be a circular dependency." ), sequence->GetDisplayName() ) );
    info.bUseLargeFont = false;
    FSlateNotificationManager::Get().AddNotification( info );

    return false;
}

FKeyPropertyResult
FCinematicBoardTrackEditor::HandleSequenceAdded( FFrameNumber iKeyTime, UMovieSceneSequence* iSequence, int32 iRowIndex )
{
    FKeyPropertyResult keyPropertyResult;

    auto boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( GetSequencer().Get() );

    const FFrameRate tickResolution = iSequence->GetMovieScene()->GetTickResolution();
    const FQualifiedFrameTime innerDuration = FQualifiedFrameTime(
        UE::MovieScene::DiscreteSize( iSequence->GetMovieScene()->GetPlaybackRange() ),
        tickResolution );

    const FFrameRate outerFrameRate = boardTrack->GetTypedOuter<UMovieScene>()->GetTickResolution();
    const int32      outerDuration = innerDuration.ConvertTo( outerFrameRate ).FrameNumber.Value;

    UMovieSceneSubSection* newSection = boardTrack->AddSequenceOnRow( iSequence, iKeyTime, outerDuration, iRowIndex );
    keyPropertyResult.bTrackModified = true;

    BoardSequenceTools::UpdateViewRange( GetSequencer().Get(), newSection->GetTrueRange() );
    GetSequencer()->EmptySelection();
    GetSequencer()->SelectSection( newSection );
    GetSequencer()->ThrobSectionSelection();

    if( tickResolution != outerFrameRate )
    {
        FNotificationInfo info( FText::Format( LOCTEXT( "TickResolutionMismatch", "The parent sequence has a different tick resolution {0} than the newly added sequence {1}" ), outerFrameRate.ToPrettyText(), tickResolution.ToPrettyText() ) );
        info.bUseLargeFont = false;
        FSlateNotificationManager::Get().AddNotification( info );
    }

    return keyPropertyResult;
}


bool
FCinematicBoardTrackEditor::SupportsSequence( UMovieSceneSequence* iSequence ) const //override
{
    ETrackSupport TrackSupported = iSequence ? iSequence->IsTrackSupported( UMovieSceneCinematicBoardTrack::StaticClass() ) : ETrackSupport::NotSupported;
    return TrackSupported == ETrackSupport::Supported;
}


bool
FCinematicBoardTrackEditor::SupportsType( TSubclassOf<UMovieSceneTrack> iType ) const //override
{
    return ( iType == UMovieSceneCinematicBoardTrack::StaticClass() );
}


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
    return FEposTracksEditorStyle::Get().GetBrush( "Sequencer.Tracks.CinematicBoard" );
}

bool
FCinematicBoardTrackEditor::OnAllowDrop( const FDragDropEvent& iDragDropEvent, FSequencerDragDropParams& DragDropParams ) //override
{
    if( !DragDropParams.Track.IsValid() || !DragDropParams.Track.Get()->IsA( UMovieSceneCinematicBoardTrack::StaticClass() ) )
    {
        return false;
    }

    TSharedPtr<FDragDropOperation> operation = iDragDropEvent.GetOperation();

    if( !operation.IsValid() || !operation->IsOfType<FAssetDragDropOp>() )
    {
        return false;
    }

    TSharedPtr<FAssetDragDropOp> dragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( operation );

    for( const FAssetData& assetData : dragDropOp->GetAssets() )
    {
        if( UMovieSceneSequence* Sequence = Cast<UMovieSceneSequence>( assetData.GetAsset() ) )
        {
            FFrameRate TickResolution = GetSequencer()->GetFocusedTickResolution();

            const FQualifiedFrameTime InnerDuration = FQualifiedFrameTime(
                UE::MovieScene::DiscreteSize( Sequence->GetMovieScene()->GetPlaybackRange() ),
                Sequence->GetMovieScene()->GetTickResolution() );

            FFrameNumber LengthInFrames = InnerDuration.ConvertTo( TickResolution ).FrameNumber;
            DragDropParams.FrameRange = TRange<FFrameNumber>( DragDropParams.FrameNumber, DragDropParams.FrameNumber + LengthInFrames );
            return true;
        }
    }

    return false;
}

FReply
FCinematicBoardTrackEditor::OnDrop( const FDragDropEvent& iDragDropEvent, const FSequencerDragDropParams& DragDropParams ) //override
{
    if( !DragDropParams.Track.IsValid() || !DragDropParams.Track.Get()->IsA( UMovieSceneCinematicBoardTrack::StaticClass() ) )
    {
        return FReply::Unhandled();
    }

    TSharedPtr<FDragDropOperation> operation = iDragDropEvent.GetOperation();

    if( !operation.IsValid() || !operation->IsOfType<FAssetDragDropOp>() )
    {
        return FReply::Unhandled();
    }

    const FScopedTransaction Transaction( LOCTEXT( "DropAssets", "Drop Assets" ) );

    TSharedPtr<FAssetDragDropOp> dragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( operation );

    FMovieSceneTrackEditor::BeginKeying( DragDropParams.FrameNumber );

    bool anyDropped = false;
    for( const FAssetData& assetData : dragDropOp->GetAssets() )
    {
        UMovieSceneSequence* sequence = Cast<UMovieSceneSequence>( assetData.GetAsset() );

        if( sequence )
        {
            AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FCinematicBoardTrackEditor::AddKeyInternal, sequence, DragDropParams.RowIndex, TOptional<FFrameNumber>( DragDropParams.FrameNumber ) ) );

            anyDropped = true;
        }
    }

    return anyDropped ? FReply::Handled() : FReply::Unhandled();
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

//void
//FCinematicBoardTrackEditor::DuplicateBoard( UMovieSceneCinematicBoardSection* iSection )
//{
//    CinematicBoardTrackTools::DuplicateSection( GetSequencer().Get(), iSection );
//}

//void
//FCinematicBoardTrackEditor::CloneSection( UMovieSceneCinematicBoardSection* iSection )
//{
//    CinematicBoardTrackTools::CloneSection( GetSequencer().Get(), iSection, GetSequencer()->GetLocalTime().Time.FrameNumber );
//}

//void
//FCinematicBoardTrackEditor::RenderBoards( const TArray<UMovieSceneCinematicBoardSection*>& iSections )
//{
//    GetSequencer()->RenderMovie( iSections ); // It takes UMovieSceneCinematicSHOTSection !!!
//}

void
FCinematicBoardTrackEditor::RenameBoard( UMovieSceneCinematicBoardSection* iSection )
{
    //@todo
}

//void
//FCinematicBoardTrackEditor::NewTake( UMovieSceneCinematicBoardSection* Section )
//{
//    const FScopedTransaction Transaction( LOCTEXT( "NewTake_Transaction", "New Take" ) );
//
//    FString ShotPrefix;
//    uint32 ShotNumber = INDEX_NONE;
//    uint32 TakeNumber = INDEX_NONE;
//    if( MovieSceneToolHelpers::ParseShotName( Section->GetShotDisplayName(), ShotPrefix, ShotNumber, TakeNumber ) )
//    {
//        TArray<FAssetData> AssetData;
//        uint32 CurrentTakeNumber = INDEX_NONE;
//        MovieSceneToolHelpers::GatherTakes( Section, AssetData, CurrentTakeNumber );
//        uint32 NewTakeNumber = CurrentTakeNumber;
//
//        for( auto ThisAssetData : AssetData )
//        {
//            uint32 ThisTakeNumber = INDEX_NONE;
//            if( MovieSceneToolHelpers::GetTakeNumber( Section, ThisAssetData, ThisTakeNumber ) )
//            {
//                if( ThisTakeNumber >= NewTakeNumber )
//                {
//                    NewTakeNumber = ThisTakeNumber + 1;
//                }
//            }
//        }
//
//        FString NewShotName = MovieSceneToolHelpers::ComposeShotName( ShotPrefix, ShotNumber, NewTakeNumber );
//
//        TRange<FFrameNumber> NewShotRange = Section->GetRange();
//        FFrameNumber         NewShotStartOffset = Section->Parameters.StartFrameOffset;
//        float                NewShotTimeScale = Section->Parameters.TimeScale;
//        int32                NewShotPrerollFrames = Section->GetPreRollFrames();
//        int32                NewRowIndex = Section->GetRowIndex();
//        FFrameNumber         NewShotStartTime = NewShotRange.GetLowerBound().IsClosed() ? MovieScene::DiscreteInclusiveLower( NewShotRange ) : 0;
//
//        UMovieSceneSubSection* NewShot = CreateShotInternal( NewShotName, NewShotStartTime, Section );
//
//        if( NewShot )
//        {
//            UMovieSceneCinematicShotTrack* CinematicShotTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack(GetSequencer().Get());
//            CinematicShotTrack->RemoveSection( *Section );
//
//            NewShot->SetRange( NewShotRange );
//            NewShot->Parameters.StartFrameOffset = NewShotStartOffset;
//            NewShot->Parameters.TimeScale = NewShotTimeScale;
//            NewShot->SetPreRollFrames( NewShotPrerollFrames );
//            NewShot->SetRowIndex( NewRowIndex );
//
//            MovieSceneToolHelpers::SetTakeNumber( NewShot, NewTakeNumber );
//
//            GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
//            GetSequencer()->EmptySelection();
//            GetSequencer()->SelectSection( NewShot );
//            GetSequencer()->ThrobSectionSelection();
//        }
//    }
//}
//
//void
//FCinematicBoardTrackEditor::SwitchTake( UObject* TakeObject )
//{
//    bool bSwitchedTake = false;
//
//    const FScopedTransaction Transaction( LOCTEXT( "SwitchTake_Transaction", "Switch Take" ) );
//
//    TArray<UMovieSceneSection*> Sections;
//    GetSequencer()->GetSelectedSections( Sections );
//
//    for( int32 SectionIndex = 0; SectionIndex < Sections.Num(); ++SectionIndex )
//    {
//        if( !Sections[SectionIndex]->IsA<UMovieSceneSubSection>() )
//        {
//            continue;
//        }
//
//        UMovieSceneSubSection* Section = Cast<UMovieSceneSubSection>( Sections[SectionIndex] );
//
//        if( TakeObject && TakeObject->IsA( UMovieSceneSequence::StaticClass() ) )
//        {
//            UMovieSceneSequence* MovieSceneSequence = CastChecked<UMovieSceneSequence>( TakeObject );
//
//            UMovieSceneCinematicShotTrack* CinematicShotTrack = CastChecked<UMovieSceneCinematicShotTrack>( Section->GetOuter() );
//
//            TRange<FFrameNumber> NewShotRange = Section->GetRange();
//            FFrameNumber         NewShotStartOffset = Section->Parameters.StartFrameOffset;
//            float                NewShotTimeScale = Section->Parameters.TimeScale;
//            int32                NewShotPrerollFrames = Section->GetPreRollFrames();
//            int32                NewRowIndex = Section->GetRowIndex();
//            FFrameNumber         NewShotStartTime = NewShotRange.GetLowerBound().IsClosed() ? MovieScene::DiscreteInclusiveLower( NewShotRange ) : 0;
//            int32                NewShotRowIndex = Section->GetRowIndex();
//
//            const int32 Duration = ( NewShotRange.GetLowerBound().IsClosed() && NewShotRange.GetUpperBound().IsClosed() ) ? MovieScene::DiscreteSize( NewShotRange ) : 1;
//            UMovieSceneSubSection* NewShot = CinematicShotTrack->AddSequence( MovieSceneSequence, NewShotStartTime, Duration );
//
//            if( NewShot != nullptr )
//            {
//                CinematicShotTrack->RemoveSection( *Section );
//
//                NewShot->SetRange( NewShotRange );
//                NewShot->Parameters.StartFrameOffset = NewShotStartOffset;
//                NewShot->Parameters.TimeScale = NewShotTimeScale;
//                NewShot->SetPreRollFrames( NewShotPrerollFrames );
//                NewShot->SetRowIndex( NewShotRowIndex );
//                bSwitchedTake = true;
//            }
//        }
//    }
//
//    if( bSwitchedTake )
//    {
//        GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
//    }
//}


/* FCinematicBoardTrackEditor callbacks
 *****************************************************************************/


FKeyPropertyResult
FCinematicBoardTrackEditor::AddKeyInternal( FFrameNumber iKeyTime, UMovieSceneSequence* iMovieSceneSequence, int32 iRowIndex, TOptional<FFrameNumber> iDroppedFrame )
{
    FKeyPropertyResult keyPropertyResult;

    if( !( ( iMovieSceneSequence->GetClass()->GetName() == TEXT( "BoardSequence" ) ) || ( iMovieSceneSequence->GetClass()->GetName() == TEXT( "ShotSequence" ) ) ) )
        return keyPropertyResult;

    if( iMovieSceneSequence->GetMovieScene()->GetPlaybackRange().IsEmpty() )
    {
        FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequenceDuration", "Invalid level sequence {0}. The sequence has no duration." ), iMovieSceneSequence->GetDisplayName() ) );
        info.bUseLargeFont = false;
        FSlateNotificationManager::Get().AddNotification( info );
        return keyPropertyResult;
    }

    if( CanAddSubSequence( *iMovieSceneSequence ) )
    {
        UMovieSceneCinematicBoardTrack* boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( GetSequencer().Get() );

        const FFrameRate tickResolution = iMovieSceneSequence->GetMovieScene()->GetTickResolution();
        const FQualifiedFrameTime innerDuration = FQualifiedFrameTime(
            UE::MovieScene::DiscreteSize( iMovieSceneSequence->GetMovieScene()->GetPlaybackRange() ),
            tickResolution );

        const FFrameRate outerFrameRate = boardTrack->GetTypedOuter<UMovieScene>()->GetTickResolution();
        const int32      outerDuration = innerDuration.ConvertTo( outerFrameRate ).FrameNumber.Value;

        UMovieSceneSubSection* newSection = boardTrack->AddSequenceOnRow( iMovieSceneSequence, iDroppedFrame.IsSet() ? iDroppedFrame.GetValue() : iKeyTime, outerDuration, iRowIndex );
        keyPropertyResult.bTrackModified = true;

        BoardSequenceTools::UpdateViewRange( GetSequencer().Get(), newSection->GetTrueRange() );
        GetSequencer()->EmptySelection();
        GetSequencer()->SelectSection( newSection );
        GetSequencer()->ThrobSectionSelection();

        if( tickResolution != outerFrameRate )
        {
            FNotificationInfo info( FText::Format( LOCTEXT( "TickResolutionMismatch", "The parent sequence has a different tick resolution {0} than the newly added sequence {1}" ), outerFrameRate.ToPrettyText(), tickResolution.ToPrettyText() ) );
            info.bUseLargeFont = false;
            FSlateNotificationManager::Get().AddNotification( info );
        }

        return keyPropertyResult;
    }

    FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequence", "Invalid level sequence {0}. There could be a circular dependency." ), iMovieSceneSequence->GetDisplayName() ) );
    info.bUseLargeFont = false;
    FSlateNotificationManager::Get().AddNotification( info );

    return keyPropertyResult;
}

//---

bool
FCinematicBoardTrackEditor::CanAddSubSequence( const UMovieSceneSequence& iSequence ) const
{
    // prevent adding ourselves and ensure we have a valid movie scene
    UMovieSceneSequence* focusedSequence = GetSequencer()->GetFocusedMovieSceneSequence();

    if( ( focusedSequence == nullptr ) || ( focusedSequence == &iSequence ) || ( focusedSequence->GetMovieScene() == nullptr ) )
    {
        return false;
    }

    // ensure that the other sequence has a valid movie scene
    UMovieScene* sequenceMovieScene = iSequence.GetMovieScene();

    if( sequenceMovieScene == nullptr )
    {
        return false;
    }

    // make sure we are not contained in the other sequence (circular dependency)
    // @todo sequencer: this check is not sufficient (does not prevent circular dependencies of 2+ levels)
    UMovieSceneSubTrack* sequenceSubTrack = sequenceMovieScene->FindMasterTrack<UMovieSceneSubTrack>();
    if( sequenceSubTrack && sequenceSubTrack->ContainsSequence( *focusedSequence, true ) )
    {
        return false;
    }

    UMovieSceneCinematicBoardTrack* boardTrack = sequenceMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( boardTrack && boardTrack->ContainsSequence( *focusedSequence, true ) )
    {
        return false;
    }

    return true;
}

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
    if( iCheckBoxState == ECheckBoxState::Checked )
    {
        for( FLevelEditorViewportClient* levelVC : GEditor->GetLevelViewportClients() )
        {
            if( levelVC && levelVC->AllowsCinematicControl() && levelVC->GetViewMode() != VMI_Unknown )
            {
                levelVC->SetActorLock( nullptr );
                levelVC->bLockedCameraView = false;
                levelVC->UpdateViewForLockedActor();
                levelVC->Invalidate();
            }
        }
        GetSequencer()->SetPerspectiveViewportCameraCutEnabled( true );
    }
    else
    {
        GetSequencer()->UpdateCameraCut( nullptr, EMovieSceneCameraCutParams() );
        GetSequencer()->SetPerspectiveViewportCameraCutEnabled( false );
    }

    GetSequencer()->ForceEvaluate();
}

FText
FCinematicBoardTrackEditor::GetLockBoardsToolTip() const
{
    return AreBoardsLocked() == ECheckBoxState::Checked ?
        LOCTEXT( "UnlockBoards", "Unlock Viewport from Boards" ) :
        LOCTEXT( "LockBoards", "Lock Viewport to Boards" );
}


#undef LOCTEXT_NAMESPACE
