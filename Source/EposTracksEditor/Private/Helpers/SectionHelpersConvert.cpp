// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
