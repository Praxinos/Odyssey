// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"

class UMovieScene;
class UMovieSceneSection;
class UMovieSceneTrack;

UENUM()
enum class EArrangeSections : uint8
{
    OnOneRow            UMETA( DisplayName = "On a single row" ),
    OnTwoRowsShifted    UMETA( DisplayName = "On 2 rows shifted" ),
};

//---

class EPOSTRACKS_API FArrangeSectionsHelpers
{
public:
    static void Arrange( UMovieSceneTrack* iTrack, EArrangeSections iArrangeShots );
};
