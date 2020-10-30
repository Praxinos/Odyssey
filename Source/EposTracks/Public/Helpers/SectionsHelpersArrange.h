// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"

#include "ArrangeSectionsType.h"

class UMovieSceneTrack;

class EPOSTRACKS_API FArrangeSectionsHelpers
{
public:
    static void Arrange( UMovieSceneTrack* iTrack, EArrangeSections iArrangeShots );
};
