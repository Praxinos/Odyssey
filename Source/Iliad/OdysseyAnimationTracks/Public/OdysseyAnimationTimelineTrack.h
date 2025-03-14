// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "Tracks/MovieSceneSubTrack.h"
#include "Compilation/IMovieSceneTrackTemplateProducer.h"

#include "OdysseyAnimationTimelineTrack.generated.h"

class UOdysseyAnimation;

UCLASS()
class ODYSSEYANIMATIONTRACKS_API UOdysseyAnimationTimelineTrack
    : public UMovieSceneNameableTrack
    , public IMovieSceneTrackTemplateProducer
{
public:
    GENERATED_BODY()

    UOdysseyAnimationTimelineTrack(const FObjectInitializer& ObjectInitializer);

    virtual UMovieSceneSection* AddNewSection(FFrameNumber KeyTime, UOdysseyAnimation* iAnimation);

public:
    // UMovieSceneTrack interface
    virtual bool SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const override;
    virtual EMovieSceneTrackEasingSupportFlags SupportsEasing(FMovieSceneSupportsEasingParams& Params) const;
    virtual void RemoveAllAnimationData() override;
    virtual bool HasSection( const UMovieSceneSection& Section ) const override;
    virtual void AddSection( UMovieSceneSection& Section ) override;
    virtual void RemoveSection( UMovieSceneSection& Section ) override;
    virtual void RemoveSectionAt( int32 SectionIndex ) override;
    virtual bool IsEmpty() const override;
    virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;
    virtual bool SupportsMultipleRows() const override;
    virtual UMovieSceneSection* CreateNewSection() override;

    // IMovieSceneTrackTemplateProducer interface
    virtual FMovieSceneEvalTemplatePtr CreateTemplateForSection( const UMovieSceneSection& InSection ) const override;

#if WITH_EDITORONLY_DATA
    virtual FText GetDefaultDisplayName() const override;
#endif

private:
    /** List of all master audio sections */
    UPROPERTY()
    TArray<UMovieSceneSection*> Sections;

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

public:
    UPROPERTY()
    bool DisplayLayers = true;
};
