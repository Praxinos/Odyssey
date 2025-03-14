// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2024

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "MovieSceneSequenceExtensions2.generated.h"

class UMovieSceneSequence;

/**
 * Function library containing methods that should be hoisted onto UMovieSceneSequences for scripting purposes
 *
 * (PATCH: this should be moved (by Epic) inside the native UMovieSceneSequenceExtensions class in SequencerScripting plugin)
 */
UCLASS()
class UMovieSceneSequenceExtensions2
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Gets this sequence's display name
     *
     * @param Sequence        The sequence to use
     * @return The display name that this sequence is displayed as
     */
    UFUNCTION( BlueprintPure, Category = "Sequencer|Sequence", meta = ( ScriptMethod ) )
    static FText GetDisplayName( const UMovieSceneSequence* Sequence );
};
