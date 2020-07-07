// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "Templates/SubclassOf.h"
#include "Widgets/SWidget.h"
#include "ISequencer.h"
#include "MovieSceneTrack.h"
#include "ISequencerSection.h"
#include "ISequencerTrackEditor.h"
#include "MovieSceneTrackEditor.h"

#include "LevelVisibilityTrack/LevelVisibilityTrackEditorOriginal.h"

/**
 * A sequencer track editor for level visibility movie scene tracks.
 */
class FLevelVisibilityTrackEditor
	: public FLevelVisibilityTrackEditorOriginal
{
public:

	/**
	 * Constructor.
	 *
	 * @param InSequencer The sequencer instance to be used by this tool.
	 */
    FLevelVisibilityTrackEditor( TSharedRef<ISequencer> iSequencer );

	/** Virtual destructor. */
	virtual ~FLevelVisibilityTrackEditor() { }

	/**
	 * Creates an instance of this class.  Called by a sequencer.
	 *
	 * @param OwningSequencer The sequencer instance to be used by this tool.
	 * @return The new instance of this class.
	 */
	static TSharedRef<ISequencerTrackEditor> CreateTrackEditor( TSharedRef<ISequencer> iSequencer );

public:

	// ISequencerTrackEditor interface

	virtual bool SupportsSequence(UMovieSceneSequence* iSequence) const override;
};
