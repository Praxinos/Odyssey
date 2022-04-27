// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MovieSceneNameableTrack.h"

#include "MovieSceneNoteTrack.generated.h"

class UStoryNote;

/**
 * Handles manipulation of note.
 */
UCLASS()
class EPOSTRACKS_API UMovieSceneNoteTrack
    : public UMovieSceneNameableTrack
{
    GENERATED_UCLASS_BODY()

public:
    /** Adds a new sound cue to the audio */
    virtual UMovieSceneSection* AddNewNoteOnRow( UStoryNote* iNote, FFrameNumber iStartTime, int32 iDuration, int32 RowIndex );

    /** Adds a new sound cue on the next available/non-overlapping row */
    virtual UMovieSceneSection* AddNewNote( UStoryNote* iNote, FFrameNumber iStartTime, int32 iDuration );

    /** @return The audio sections on this track */
    const TArray<UMovieSceneSection*>& GetNoteSections() const;

    /** @return true if this is a master audio track */
    bool IsAMasterTrack() const;

public:
    // UMovieSceneTrack interface

    virtual bool SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const override;
    virtual void RemoveAllAnimationData() override;
    virtual bool HasSection( const UMovieSceneSection& Section ) const override;
    virtual void AddSection( UMovieSceneSection& Section ) override;
    virtual void RemoveSection( UMovieSceneSection& Section ) override;
    virtual void RemoveSectionAt( int32 SectionIndex ) override;
    virtual bool IsEmpty() const override;
    virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;
    virtual bool SupportsMultipleRows() const override;
    virtual UMovieSceneSection* CreateNewSection() override;

#if WITH_EDITORONLY_DATA
    virtual FText GetDefaultDisplayName() const override;
#endif

private:
    /** List of all master audio sections */
    UPROPERTY()
    TArray<UMovieSceneSection*> NoteSections;

#if WITH_EDITORONLY_DATA

public:
    /**
     * Get the height of this track's rows
     */
    int32 GetRowHeight() const;

    /**
     * Set the height of this track's rows
     */
    void SetRowHeight( int32 NewRowHeight );

private:
    /** The height for each row of this track */
    UPROPERTY()
    int32 RowHeight;

#endif
};
