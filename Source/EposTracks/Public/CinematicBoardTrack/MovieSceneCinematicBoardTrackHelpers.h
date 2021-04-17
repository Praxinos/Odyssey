// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "ArrangeSectionsType.h"

class UMovieSceneSection;

class EPOSTRACKS_API MovieSceneCinematicBoardTrackHelpers
{
public:
    static void Arrange( TArray< UMovieSceneSection* > ioSections, EArrangeSections iArrangeShots );

    static void Arrange( TArray< UMovieSceneSection* > ioSections );

public:
    /** Move all sections to make consecutives each other (previous end bound == next start bound) by the order inside the array */
    static void OrganizeSections( TArray< UMovieSceneSection* > iSections );
};
