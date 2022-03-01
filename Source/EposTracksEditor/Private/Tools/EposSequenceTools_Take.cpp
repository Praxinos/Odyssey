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
    //TODO: add undo ............................................................................

    return nullptr;
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

    const FScopedTransaction transaction( LOCTEXT( "SwitchTake", "Switch Take" ) );

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

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );

    return old_sequence;
}

//---

#undef LOCTEXT_NAMESPACE
