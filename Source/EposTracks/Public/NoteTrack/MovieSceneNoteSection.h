// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Sections/MovieSceneStringSection.h"
#include "Sections/MovieSceneFloatSection.h"
#include "MovieSceneNoteSection.generated.h"


/**
 * A single note section.
 */
UCLASS( MinimalAPI )
class UMovieSceneNoteSection
    : public UMovieSceneSection
{
    GENERATED_BODY()

    /** Default constructor. */
    UMovieSceneNoteSection();

public:

    /** Float data */
    UPROPERTY()
    FMovieSceneStringChannel StringCurve;

    UPROPERTY()
    FMovieSceneStringChannel TestCurve;
    UPROPERTY()
    FMovieSceneFloatChannel Test2Curve;
};
