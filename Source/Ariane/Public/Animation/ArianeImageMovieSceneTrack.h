// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

#include "CoreMinimal.h"
#include "Tracks/MovieScenePropertyTrack.h"
#include "Compilation/IMovieSceneTrackTemplateProducer.h"
#include "ArianeImageMovieSceneSection.h"
#include "EntitySystem/TrackInstance/MovieSceneTrackInstance.h"

#include "ArianeImageMovieSceneTrack.generated.h"

UCLASS()
class ARIANE_API UArianeImageMovieSceneTrack : public UMovieScenePropertyTrack, public IMovieSceneTrackTemplateProducer
{
    GENERATED_BODY()

public:
    UArianeImageMovieSceneTrack();

    // --- Interface UMovieSceneTrack ---
    virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
    virtual UMovieSceneSection* CreateNewSection() override;

    // implements IMovieSceneTrackTemplateProducer::CreateTemplateForSection
    virtual FMovieSceneEvalTemplatePtr CreateTemplateForSection(const UMovieSceneSection& InSection) const override;

    UArianeLayerDrawing* GetDrawingLayer( ISequencer& InSequencer );
};
