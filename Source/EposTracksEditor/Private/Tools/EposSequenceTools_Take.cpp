// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/EposSequenceTools.h"

#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CineCameraActor.h"
#include "IAssetTools.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieSceneTimeHelpers.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "PlaneActor.h"
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

    if( BoardSequenceTools::IsDrawingInEditionMode( iSequencer, iSubSection ) )
        return nullptr;

    const FScopedTransaction transaction( LOCTEXT( "transaction.create-take", "Create Take" ) );

    //---

    FString sequence_path;
    FString sequence_name;
    FShotNameElements shot_name_elements;
    FString sequence_pathname = NamingConvention::GenerateTakeAssetPathName( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iSequencer->GetFocusedMovieSceneSequence(), &iSubSection, sequence_path, sequence_name, shot_name_elements );

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

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );

    ShotSequenceTools::CloneInnerContent( iSequencer, result.mInnerSequence, result.mInnerSequenceId, false );

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
BoardSequenceTools::SwitchTake( ISequencer* iSequencer, UMovieSceneSubSection& iSubSection, FBoardSectionTake* iTake )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( &iSubSection );
    FBoardSectionTake* old_take = board_section->FindTake( board_section->GetSequence() );

    if( !iTake || !iTake->GetSequence().IsValid() )
        return nullptr;

    if( !board_section->FindTake( *iTake ) )
        return nullptr;

    if( BoardSequenceTools::IsDrawingInEditionMode( iSequencer, iSubSection ) )
        return nullptr;

    //---

    const FScopedTransaction transaction( LOCTEXT( "transaction.switch-take", "Switch Take" ) );

    //---

    // Pre/Post are required (I don't know really why), otherwise the section doesn't 'recognize' the new take.
    // It seems that the sequence id of the subsection, won't match the one inside the hierarchy (iPlayer.GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy())
    // And adding Pre/Post seems to solve the problem

    FProperty* ChangedProperty = FindFProperty<FProperty>( UMovieSceneCinematicBoardSection::StaticClass(), "SubSequence" );
    board_section->PreEditChange( ChangedProperty );

    board_section->SetSequence( iTake->GetSequence().Get() );

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

    return old_take;
}

//---

#undef LOCTEXT_NAMESPACE
