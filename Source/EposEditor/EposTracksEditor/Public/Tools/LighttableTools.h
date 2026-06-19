// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    static void Activate( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding );
    static void Deactivate( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding );
    static int8 GetState( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding );
    // Inside shot
    static void Activate( ISequencer* iSequencer, FGuid iAnimationBinding );
    static void Deactivate( ISequencer* iSequencer, FGuid iAnimationBinding );
    static int8 GetState( ISequencer* iSequencer, FGuid iAnimationBinding );
    // All
    static void Deactivate( ISequencer* iSequencer );

public:
    static void Activate( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding );
    static void Deactivate( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding );

    /**
     * Create a animation in a subsection at the given frame
     * @return 1 is for 'all drawings on', 0 is for 'all drawings off', -1 is for undetermined (mix of on/off)
     */
    static int8 GetState( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding );

    static bool IsOn( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding );
    static bool IsOff( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding );
};
