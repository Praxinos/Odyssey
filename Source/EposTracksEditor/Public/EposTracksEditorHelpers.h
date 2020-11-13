// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "ArrangeSectionsType.h"

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
     * Insert Shot.
     *
     * @param iSequencer The sequencer
     * @param iFrameNumber The start frame of the new board
     */
    static void InsertShot( ISequencer* iSequencer, FFrameNumber iFrameNumber );

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
    static void DuplicateSection( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection );

private:

    /**
     * Generate a new board package
     *
     * @param SequenceMovieScene The sequence movie scene for the new board
     * @param NewBoardName The new board name
     * @return The new board path
     */
    static FString GenerateNewSequencePath( UMovieScene* iRootMovieScene, UMovieScene* iFocusedMovieScene, FString& ioNewBoardName );

    /**
     * Generate a new board name
     *
     * @param AllSections All the sections in the given board track
     * @param Time The time to generate the new board name at
     * @return The new board name
     */
    template<typename SequenceClass>
    static FString GenerateNewSectionName( const TArray<UMovieSceneSection*>& iAllSections, FFrameNumber iTime );

    /*
     * Create board
     *
     * @param iSequencer The sequencer
     * @param NewBoardName The new board name.
     * @param NewBoardStartTime The time to start the new board at.
     * @param BoardToDuplicate The board to duplicate.
     * @return The new board.
     */
    template<typename SequenceClass>
    static UMovieSceneSubSection* CreateSequenceInternal( ISequencer* iSequencer, FString& ioNewSequenceName, FFrameNumber iNewSectionStartTime, UMovieSceneCinematicBoardSection* iSectionToDuplicate = nullptr );

    /*
     * Insert Board.
     *
     * @param iSequencer The sequencer
     * @param iFrameNumber The start frame of the new board
     */
    template<typename SequenceClass>
    static void InsertSequence( ISequencer* iSequencer, FFrameNumber iFrameNumber );

public:

    /*
     * Arrange sections of board track.
     *
     * @param iSequencer The sequencer
     * @param Section The section to duplicate
     */
    static void ArrangeSections( ISequencer* iSequencer );

    /*
     * Set 'arrange sections' settings.
     *
     * @param iSequencer The sequencer
     * @param iArrangeSections The new value
     */
    static void SetArrangeSections( ISequencer* iSequencer, EArrangeSections iArrangeSections );
};
