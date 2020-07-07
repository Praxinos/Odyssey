// Copyright Epic Games, Inc. All Rights Reserved.

#include "LevelVisibilityTrack/LevelVisibilityTrackEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EditorStyleSet.h"
#include "Sections/MovieSceneLevelVisibilitySection.h"
//#include "LevelVisibilityTrack/LevelVisibilitySectionOriginal.h"
#include "Tracks/MovieSceneLevelVisibilityTrack.h"
#include "SequencerUtilities.h"

FLevelVisibilityTrackEditor::FLevelVisibilityTrackEditor( TSharedRef<ISequencer> iSequencer )
	: FLevelVisibilityTrackEditorOriginal( iSequencer )
{ }


TSharedRef<ISequencerTrackEditor> FLevelVisibilityTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> iSequencer )
{
	return MakeShareable( new FLevelVisibilityTrackEditor( iSequencer ) );
}

bool FLevelVisibilityTrackEditor::SupportsSequence(UMovieSceneSequence* iSequence) const
{
    return ( iSequence != nullptr ) && ( ( iSequence->GetClass()->GetName() == TEXT( "BoardSequence" ) ) || ( iSequence->GetClass()->GetName() == TEXT( "ShotSequence" ) ) );
}
