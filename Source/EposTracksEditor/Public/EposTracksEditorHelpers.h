// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"

class ISequencer;
class UMovieScene;
class UMovieSceneSection;
class UMovieSceneSubSection;
class UMovieSceneCinematicBoardSection;
class UMovieSceneCinematicBoardTrack;

class EPOSTRACKSEDITOR_API EposTracksEditorHelpers
{
public:

    /** Find or create a cinematic board track in the currently focused movie scene. */
    static UMovieSceneCinematicBoardTrack* FindOrCreateCinematicBoardTrack( ISequencer* iSequencer );

    /*
     * Insert Board.
     *
     * @param iSequencer The sequencer
     * @param iFrameNumber The start frame of the new board
     */
    static void InsertBoard( ISequencer* iSequencer, FFrameNumber iFrameNumber );

    /*
     * Insert Filler.
     *
     * @param iSequencer The sequencer
     */
    static void InsertFiller( ISequencer* iSequencer );

    /*
     * Duplicate board.
     *
     * @param iSequencer The sequencer
     * @param Section The section to duplicate
     */
    static void DuplicateBoard( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection );

private:

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

    /*
     * Create board
     *
     * @param iSequencer The sequencer
     * @param NewBoardName The new board name.
     * @param NewBoardStartTime The time to start the new board at.
     * @param BoardToDuplicate The board to duplicate.
     * @return The new board.
     */
    static UMovieSceneSubSection* CreateBoardInternal( ISequencer* iSequencer, FString& ioNewBoardName, FFrameNumber iNewBoardStartTime, UMovieSceneCinematicBoardSection* iBoardToDuplicate = nullptr );

};