// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
    void SetNote( UStoryNote* iNote )
    {
        Note = iNote;
    }

    /** Gets the text for this section */
    UFUNCTION( BlueprintPure, Category = "Sequencer|Section" )
    UStoryNote* GetNote() const
    {
        return Note;
    }

private:
    /** The text that this section manage */
    UPROPERTY( EditAnywhere, Category = "Note" )
    UStoryNote* Note;
};
