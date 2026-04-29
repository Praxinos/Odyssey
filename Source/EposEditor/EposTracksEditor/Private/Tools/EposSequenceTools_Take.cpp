// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/EposSequenceTools.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CineCameraActor.h"
#include "Framework/Notifications/NotificationManager.h"
#include "IAssetTools.h"
#include "LevelEditorSubsystem.h"
#include "LevelUtils.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieSceneTimeHelpers.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "Settings/EposTracksSettings.h"
#include "Settings/NamingConventionSettings.h"
#include "Shot/ShotSequence.h"
#include "StoryNote.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Take"

//---

//static
FBoardSectionTake*
BoardSequenceTools::CreateTake( ISequencer* iSequencer, UMovieSceneSubSection& iSubSection )
{
    UMovieSceneSequence* subsequence = iSubSection.GetSequence();
    if( !subsequence )
        return nullptr;
    if( subsequence->IsA<UBoardSequence>() )
        return nullptr;

    if( BoardSequenceTools::IsAnimationInEditionMode( iSequencer, iSubSection ) )
        return nullptr;

    FMovieSceneSequenceID epos_sequence_id;
    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( BoardSequenceHelpers::FindSequenceOfSubSection( *iSequencer, iSubSection, epos_sequence_id ) );
    if( !epos_sequence )
        return nullptr;

    ULevelEditorSubsystem* levelEditorSubsystem = GEditor->GetEditorSubsystem<ULevelEditorSubsystem>();
    if( FLevelUtils::IsLevelLocked( levelEditorSubsystem->GetCurrentLevel() ) )
    {
        FNotificationInfo Info( LOCTEXT( "cant-create-take-in-locked-level", "The requested operation could not be completed because the level is locked." ) );
        Info.ExpireDuration = 5.0f;
        TSharedPtr<SNotificationItem> notification = FSlateNotificationManager::Get().AddNotification( Info );
        if (notification)
            notification->SetCompletionState( SNotificationItem::CS_Fail );
        return nullptr;
    }

    const FScopedTransaction transaction( LOCTEXT( "transaction.create-take", "Create Take" ) );

    //---

    FString sequence_path;
    FString sequence_name;
    FShotNameElements shot_name_elements;
    FString sequence_pathname = NamingConvention::GenerateTakeAssetPathName( *iSequencer, *epos_sequence, epos_sequence_id, &iSubSection, sequence_path, sequence_name, shot_name_elements );

    IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();
    UObject* newAsset = assetTools.DuplicateAsset( sequence_name, sequence_path, subsequence );
    UShotSequence* shot_sequence = CastChecked<UShotSequence>( newAsset );

    if( !shot_sequence )
        return nullptr;

    shot_sequence->NameElements = shot_name_elements;

    FBoardSectionTake take( shot_sequence );

    //---

    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( &iSubSection );
    board_section->AddTake( take );

    BoardSequenceTools::SwitchTake( iSequencer, iSubSection, board_section->FindTake( take ) );

    //---

    bool was_piloting_camera = BoardSequenceTools::IsPilotingCamera( iSequencer, iSubSection );
    if( was_piloting_camera )
        BoardSequenceTools::EjectCamera( iSequencer, iSubSection, iSubSection.GetInclusiveStartFrame() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );

    ShotSequenceTools::CloneInnerContent( iSequencer, result.mInnerSequence, result.mInnerSequenceId, false );

    if( was_piloting_camera )
        BoardSequenceTools::PilotCamera( iSequencer, iSubSection, iSubSection.GetInclusiveStartFrame() );

    //---

    ////new_section->SetRange( iSection->GetRange() );
    ////new_section->SetRowIndex( MovieSceneToolHelpers::FindAvailableRowIndex( boardTrack, new_section ) );
    //new_section->Parameters.StartFrameOffset = iSection->Parameters.StartFrameOffset;
    //new_section->Parameters.TimeScale = iSection->Parameters.TimeScale;
    //new_section->SetPreRollFrames( iSection->GetPreRollFrames() );

    //iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );
    //BoardSequenceTools::UpdateViewRange( iSequencer, new_section ? new_section->GetTrueRange() : TRange<FFrameNumber>::Empty() );
    iSequencer->EmptySelection();
    iSequencer->SelectSection( &iSubSection );
    iSequencer->ThrobSectionSelection();

    return board_section->FindTake( take );
}

//static
FBoardSectionTake*
BoardSequenceTools::SwitchTake( ISequencer* iSequencer, UMovieSceneSubSection& iSubSection, const FBoardSectionTake* iTake )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( &iSubSection );
    FBoardSectionTake* old_take = board_section->FindTake( board_section->GetSequence() );

    if( !iTake || !iTake->GetSequence() )
        return nullptr;

    if( !board_section->FindTake( *iTake ) )
        return nullptr;

    if( BoardSequenceTools::IsAnimationInEditionMode( iSequencer, iSubSection ) )
        return nullptr;

    bool was_piloting_camera = BoardSequenceTools::IsPilotingCamera( iSequencer, iSubSection );
    if( was_piloting_camera )
        BoardSequenceTools::EjectCamera( iSequencer, iSubSection, iSubSection.GetInclusiveStartFrame() );

    //---

    const FScopedTransaction transaction( LOCTEXT( "transaction.switch-take", "Switch Take" ) );

    //---

    // Pre/Post are required (I don't know really why), otherwise the section doesn't 'recognize' the new take.
    // It seems that the sequence id of the subsection, won't match the one inside the hierarchy (iPlayer.GetSharedPlaybackState()->GetHierarchy())
    // And adding Pre/Post seems to solve the problem

    FProperty* ChangedProperty = FindFProperty<FProperty>( UMovieSceneCinematicBoardSection::StaticClass(), "SubSequence" );
    board_section->PreEditChange( ChangedProperty );

    board_section->SetSequence( iTake->GetSequence() );

    FPropertyChangedEvent PropertyChangedEvent( ChangedProperty );
    //CameraComponent->PostEditChangeProperty( PropertyChangedEvent );
    FEditPropertyChain PropertyChain;
    PropertyChain.AddHead( ChangedProperty );
    FPropertyChangedChainEvent PropertyChainEvent( PropertyChain, PropertyChangedEvent );
    board_section->PostEditChangeChainProperty( PropertyChainEvent );

    //---

    int32 new_duration = UE::MovieScene::DiscreteSize( iTake->GetSequence()->GetMovieScene()->GetPlaybackRange() );
    TRange<FFrameNumber> new_range = UE::MovieScene::MakeDiscreteRangeFromLower( board_section->GetTrueRange().GetLowerBound(), new_duration );

    board_section->StartResizing();
    board_section->ResizeTrailingEdge( new_range.GetUpperBoundValue() );
    if( board_section->IsResizingTrailing() )
    {
        board_section->Resizing(); // So OnSectionMoved() will go inside IsResizing()

        UMovieSceneCinematicBoardTrack* track = board_section->GetTypedOuter<UMovieSceneCinematicBoardTrack>();
        track->OnSectionMoved( *board_section, EPropertyChangeType::ValueSet ); // This will call StopResizing()
    }
    else
    {
        board_section->StopResizing();
    }

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );

    if( was_piloting_camera )
        BoardSequenceTools::PilotCamera( iSequencer, iSubSection, iSubSection.GetInclusiveStartFrame() );

    return old_take;
}

//---

#undef LOCTEXT_NAMESPACE
