// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "Templates/SubclassOf.h"
#include "Widgets/SWidget.h"
#include "ISequencerSection.h"
#include "MovieSceneTrack.h"
#include "ISequencer.h"
#include "ISequencerTrackEditor.h"
#include "MovieSceneTrackEditor.h"
#include "IContentBrowserSingleton.h"

#include "AudioTrack/AudioTrackEditorOriginal.h"

/**
 * Tools for audio tracks
 */
class EPOSTRACKSEDITOR_API FAudioTrackEditor
	: public FAudioTrackEditorOriginal
{
public:

	/**
	 * Constructor
	 *
	 * @param InSequencer The sequencer instance to be used by this tool
	 */
    FAudioTrackEditor(TSharedRef<ISequencer> iSequencer);

	/** Virtual destructor. */
	virtual ~FAudioTrackEditor();

	/**
	 * Creates an instance of this class.  Called by a sequencer 
	 *
	 * @param OwningSequencer The sequencer instance to be used by this tool
	 * @return The new instance of this class
	 */
	static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> iSequencer);

public:

	// ISequencerTrackEditor interface

	virtual bool SupportsSequence(UMovieSceneSequence* iSequence) const override;
};
