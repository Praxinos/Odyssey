// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "MovieSceneSequenceID.h"

#include "OdysseyAnimation.h"

class ISequencer;
class IMovieScenePlayer;
class UMovieSceneSequence;
class UOdysseyAnimation;
class UStoryNote;

/** The grid type which must correspond to the value inside the material instance */
enum class EGridType
{
    kNone = 0,
    k2x2,
    k3x3,
    kCrosshair,
    kAbatment,
};

/*
 * The management of project assets
 */
class EPOSTRACKSEDITOR_API ProjectAssetTools
{
public:
    static UOdysseyAnimation* CreateAnimation( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FIntPoint iSize, EOdysseyAnimationFormat iFormat, FFrameRate iFrameRate, TSubclassOf<class UOdysseyAnimationLayer> iDefaultLayerClass, TOptional<FLinearColor> iLayerBackgroundColor );

    static UStoryNote* CreateNote( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static UStoryNote* CloneNote( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UStoryNote* iNoteToClone );
};
