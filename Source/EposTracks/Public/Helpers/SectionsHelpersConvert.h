// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/FrameTime.h"

class UMovieSceneSubSection;

class EPOSTRACKS_API SectionsHelpersConvert
{
public:
    /** Convert a key inside a subsequence reference to its (a) parent reference */
    static FFrameTime InnerToOuter( const UMovieSceneSubSection* iOuterSection, FFrameTime iInnerKeys );
    /** Convert all keys inside a subsequence reference to its (a) parent reference */
    static TArray<FFrameTime> InnerToOuter( const UMovieSceneSubSection* iOuterSection, TArray<FFrameTime> iInnerKeys );

    /** Convert a key from frame to second */
    static double FrameToSecond( const UMovieSceneSection* iSection, FFrameTime iFrameKeys );
    /** Convert all keys from frame to second */
    static TArray<double> FrameToSecond( const UMovieSceneSection* iSection, TArray<FFrameTime> iFrameKeys );
};
