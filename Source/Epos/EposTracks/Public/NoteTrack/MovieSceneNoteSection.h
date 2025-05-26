// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Sections/MovieSceneStringSection.h"
#include "Sections/MovieSceneFloatSection.h"

#include "MovieSceneNoteSection.generated.h"

class UStoryNote;

/**
 * A single note section.
 */
UCLASS( MinimalAPI )
class UMovieSceneNoteSection
    : public UMovieSceneSection
{
    GENERATED_UCLASS_BODY()

public:
    /** Sets this section's text */
    UFUNCTION( BlueprintCallable, Category = "Sequencer|Section" )
    EPOSTRACKS_API void SetNote( UStoryNote* Note );

    /** Gets the text for this section */
    UFUNCTION( BlueprintPure, Category = "Sequencer|Section" )
    EPOSTRACKS_API UStoryNote* GetNote() const;

private:
    /** The text that this section manage */
    UPROPERTY( EditAnywhere, Category = "Note" )
    UStoryNote* Note;
};
