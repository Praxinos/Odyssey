// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoardSequenceHelpers.h"

#include "ISequencer.h"
#include "MovieScene.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"

#include "EposTracksEditorHelpers.h"
#include "Helpers/SectionsHelpersArrange.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Settings/EposEditorSettings.h"

#define LOCTEXT_NAMESPACE "BoardSequenceHelpers_Sections"

//static
void
BoardSequenceHelpers::ArrangeSections( ISequencer* iSequencer )
{
    auto track = iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( !track )
        return;

    const UEposEditorSettings* settings = GetDefault<UEposEditorSettings>();

    FArrangeSectionsHelpers::Arrange( track, settings->BoardTrackSettings.ArrangeShots );

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
}

//static
void
BoardSequenceHelpers::SetArrangeSections( ISequencer* iSequencer, EArrangeSections iArrangeSections )
{
    UEposEditorSettings* settings = GetMutableDefault<UEposEditorSettings>();
    settings->BoardTrackSettings.ArrangeShots = iArrangeSections;
    settings->SaveConfig();

    BoardSequenceHelpers::ArrangeSections( iSequencer );
}

//static
void
BoardSequenceHelpers::NewSectionWithBoardAtCurrentFrame( ISequencer* iSequencer )
{
    EposTracksEditorHelpers::InsertBoard( iSequencer, iSequencer->GetLocalTime().Time.FrameNumber );
}

#undef LOCTEXT_NAMESPACE
