// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "Tracks/MovieSceneSubTrack.h"
#include "Compilation/IMovieSceneTrackTemplateProducer.h"

#include "OdysseyAnimationTimelineTrack.generated.h"

UCLASS()
class ODYSSEYANIMATIONTRACKS_API UOdysseyAnimationTimelineTrack
    : public UMovieSceneSubTrack
    , public IMovieSceneTrackTemplateProducer
{
public:
    GENERATED_BODY()

    UOdysseyAnimationTimelineTrack(const FObjectInitializer& ObjectInitializer);

    virtual UMovieSceneSection* AddNewSection(FFrameNumber KeyTime, float iDurationInSeconds);

    // UMovieSceneTrack interface
    virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
    virtual EMovieSceneTrackEasingSupportFlags SupportsEasing(FMovieSceneSupportsEasingParams& Params) const;
    virtual bool SupportsMultipleRows() const override;
    virtual UMovieSceneSection* CreateNewSection() override;
    virtual FMovieSceneEvalTemplatePtr CreateTemplateForSection(const UMovieSceneSection& InSection) const override;

#if WITH_EDITORONLY_DATA
    virtual FText GetDisplayName() const override;
#endif

public:
    UPROPERTY()
    bool DisplayLayers = true;
};
