// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
