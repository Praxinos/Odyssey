// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/FrameTime.h"

class UMovieSceneSection;
class UMovieSceneSubSection;

class SectionHelpersConvert
{
public:
    /** Convert all keys from frame to second */
    static TArray<double> FrameToSecond( const UMovieSceneSection* iSection, TArray<FFrameTime> iFrameKeys );
};
