// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"

class UMovieScene;
class UMovieSceneSection;

class EPOSTRACKSEDITOR_API EposTracksEditorHelpers
{
public:

    /**
     * Generate a new board package
     *
     * @param SequenceMovieScene The sequence movie scene for the new board
     * @param NewBoardName The new board name
     * @return The new board path
     */
    static FString GenerateNewBoardPath( UMovieScene* iSequenceMovieScene, FString& ioNewBoardName );

    /**
     * Generate a new board name
     *
     * @param AllSections All the sections in the given board track
     * @param Time The time to generate the new board name at
     * @return The new board name
     */
    static FString GenerateNewBoardName( const TArray<UMovieSceneSection*>& iAllSections, FFrameNumber iTime );
};