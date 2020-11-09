// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "ArrangeSectionsType.h"

class UMovieSceneTrack;

class EPOSTRACKS_API SectionsHelpersArrange
{
public:
    static void Arrange( UMovieSceneTrack* iTrack, EArrangeSections iArrangeShots );
};
