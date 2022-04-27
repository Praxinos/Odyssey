// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Helpers/SectionHelpersConvert.h"

#include "MovieScene.h"
#include "Sections/MovieSceneSubSection.h"

//---

//static
TArray<double>
SectionHelpersConvert::FrameToSecond( const UMovieSceneSection* iSection, TArray<FFrameTime> iFrameKeys )
{
    TArray<double> second_keys;

    for( auto key : iFrameKeys )
    {
        FQualifiedFrameTime time( key, iSection->GetTypedOuter<UMovieScene>()->GetTickResolution() );
        second_keys.Add( time.AsSeconds() );
    }

    return second_keys;
}
