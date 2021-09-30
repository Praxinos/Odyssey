// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
//#include "Tracks/MovieSceneStringTrack.h"
//#include "Tracks/MovieSceneFloatTrack.h"
#include "Tracks/MovieScenePropertyTrack.h"
#include "MovieSceneNoteTrack.generated.h"


/**
 * Implements a movie scene track that holds a series of strings.
 */
UCLASS( MinimalAPI )
class UMovieScenePatchStringTrack
    : public UMovieScenePropertyTrack
    //, public IMovieSceneTrackTemplateProducer
{
    GENERATED_UCLASS_BODY()

public:

//    /** Default constructor. */
//    UMovieSceneStringTrack()
//    {
//#if WITH_EDITORONLY_DATA
//        TrackTint = FColor( 128, 128, 128 );
//#endif
//    }

public:

    //~ UMovieSceneTrack interface

    virtual void AddSection( UMovieSceneSection& Section ) override;
    //virtual bool SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const override;
    //virtual UMovieSceneSection* CreateNewSection() override;
    //virtual FMovieSceneEvalTemplatePtr CreateTemplateForSection( const UMovieSceneSection& InSection ) const override;
    virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;
    virtual bool HasSection( const UMovieSceneSection& Section ) const override;
    virtual bool IsEmpty() const override;
    virtual void RemoveAllAnimationData() override;
    virtual void RemoveSection( UMovieSceneSection& Section ) override;
    virtual void RemoveSectionAt( int32 SectionIndex ) override;
};

/**
 * Handles manipulation of note.
 */
UCLASS()
class EPOSTRACKS_API UMovieSceneNoteTrack
    : public UMovieScenePatchStringTrack
    //: public UMovieSceneStringTrack
    //: public UMovieSceneFloatTrack
{
    GENERATED_BODY()

public:

    //UMovieSceneNoteTrack();
    UMovieSceneNoteTrack( const FObjectInitializer& Init );

    // UMovieSceneTrack interface

    virtual bool SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const override;
    virtual UMovieSceneSection* CreateNewSection() override;

#if WITH_EDITORONLY_DATA
    virtual FText GetDefaultDisplayName() const override;
    virtual bool CanRename() const override
    {
        return true;
    }
#endif

#if WITH_EDITORONLY_DATA

public:

    /**
     * Get the height of this track's rows
     */
    int32 GetRowHeight() const
    {
        return RowHeight;
    }

    /**
     * Set the height of this track's rows
     */
    void SetRowHeight(int32 NewRowHeight)
    {
        RowHeight = FMath::Max(16, NewRowHeight);
    }

private:

    /** The height for each row of this track */
    UPROPERTY()
    int32 RowHeight;

#endif
};
