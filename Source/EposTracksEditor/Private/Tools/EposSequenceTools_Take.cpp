// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
UShotSequence*
BoardSequenceTools::CreateTake( ISequencer* iSequencer, UMovieSceneSubSection& iSubSection )
{
    UMovieSceneSequence* subsequence = iSubSection.GetSequence();
    if( !subsequence )
        return nullptr;
    if( subsequence->IsA<UBoardSequence>() )
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

    //---

    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( &iSubSection );
    board_section->AddTake( shot_sequence );

    BoardSequenceTools::SwitchTake( iSequencer, iSubSection, shot_sequence );

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

    return shot_sequence;
}

//static
UShotSequence*
BoardSequenceTools::SwitchTake( ISequencer* iSequencer, UMovieSceneSubSection& iSubSection, UShotSequence* iTake )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( &iSubSection );
    UShotSequence* old_sequence = Cast<UShotSequence>( board_section->GetSequence() );

    if( !board_section->GetTakes().Contains( iTake ) )
        return nullptr;

    //---

    const FScopedTransaction transaction( LOCTEXT( "transaction.switch-take", "Switch Take" ) );

    //---

    // Pre/Post are required (I don't know really why), otherwise the section doesn't 'recognize' the new take.
    // It seems that the sequence id of the subsection, won't match the one inside the hierarchy (iPlayer.GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy())
    // And adding Pre/Post seems to solve the problem

    FProperty* ChangedProperty = FindFProperty<FProperty>( UMovieSceneCinematicBoardSection::StaticClass(), "SubSequence" );
    board_section->PreEditChange( ChangedProperty );

    board_section->SetSequence( iTake );

    FPropertyChangedEvent PropertyChangedEvent( ChangedProperty );
    //CameraComponent->PostEditChangeProperty( PropertyChangedEvent );
    FEditPropertyChain PropertyChain;
    PropertyChain.AddHead( ChangedProperty );
    FPropertyChangedChainEvent PropertyChainEvent( PropertyChain, PropertyChangedEvent );
    board_section->PostEditChangeChainProperty( PropertyChainEvent );

    //---

    //iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );

    return old_sequence;
}

//---

#undef LOCTEXT_NAMESPACE
