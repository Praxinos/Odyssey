// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class UMovieScene;
class UMovieSceneSection;

class EPOSTRACKS_API SectionsHelpersResize
{
public:
    static TRange<FFrameNumber> GetValidRangeLeading( TArray<UMovieSceneSection*> iSections, UMovieSceneSection* iSection, FFrameNumber iNewFrame, int32 iThreshold );
    static TRange<FFrameNumber> GetValidRangeTrailing( TArray<UMovieSceneSection*> iSections, UMovieSceneSection* iSection, FFrameNumber iNewFrame, int32 iThreshold );

    static UMovieSceneSection* FixupConsecutiveSections( TArray<UMovieSceneSection*> iSections, UMovieSceneSection* iSection );
};
