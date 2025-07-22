// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
