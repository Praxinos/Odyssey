// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "MovieSceneSequenceID.h"

class ISequencer;
class UMovieSceneSequence;
class UMovieSceneSubSection;

/*
 * The lighttable management
 */
class EPOSTRACKSEDITOR_API LighttableTools
{
public:
    // Inside board
    static void Activate( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding );
    static void Deactivate( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding );
    static int8 GetState( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding );
    // Inside shot
    static void Activate( ISequencer* iSequencer, FGuid iPlaneBinding );
    static void Deactivate( ISequencer* iSequencer, FGuid iPlaneBinding );
    static int8 GetState( ISequencer* iSequencer, FGuid iPlaneBinding );
    // All
    static void Deactivate( ISequencer* iSequencer );

public:
    static void Activate( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding );
    static void Deactivate( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding );

    static void Update( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding );

    /**
     * Create a plane in a subsection at the given frame
     * @return 1 is for 'all drawings on', 0 is for 'all drawings off', -1 is for undetermined (mix of on/off)
     */
    static int8 GetState( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding );

    static bool IsOn( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding );
    static bool IsOff( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding );
};
