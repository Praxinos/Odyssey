// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Helpers/SectionsHelpersConvert.h"

#include "MovieScene.h"
#include "Sections/MovieSceneSubSection.h"

//---

//static
FFrameTime
SectionsHelpersConvert::InnerToOuter( const UMovieSceneSubSection* iOuterSection, FFrameTime iInnerKey )
{
    const FMovieSceneSequenceTransform InnerToOuterTransform = iOuterSection->OuterToInnerTransform().InverseLinearOnly();
    return iInnerKey * InnerToOuterTransform;
}

//static
TArray<FFrameTime>
SectionsHelpersConvert::InnerToOuter( const UMovieSceneSubSection* iOuterSection, TArray<FFrameTime> iInnerKeys )
{
    TArray<FFrameTime> converted_keys;

    const FMovieSceneSequenceTransform InnerToOuterTransform = iOuterSection->OuterToInnerTransform().InverseLinearOnly();
    for( auto key : iInnerKeys )
    {
        const FFrameTime converted_key = key * InnerToOuterTransform;
        converted_keys.Add( converted_key );
    }

    return converted_keys;
}

//static
double
SectionsHelpersConvert::FrameToSecond( const UMovieSceneSection* iSection, FFrameTime iFrameKey )
{
    FQualifiedFrameTime time( iFrameKey, iSection->GetTypedOuter<UMovieScene>()->GetTickResolution() );

    return time.AsSeconds();
}

//static
TArray<double>
SectionsHelpersConvert::FrameToSecond( const UMovieSceneSection* iSection, TArray<FFrameTime> iFrameKeys )
{
    TArray<double> second_keys;

    for( auto key : iFrameKeys )
    {
        FQualifiedFrameTime time( key, iSection->GetTypedOuter<UMovieScene>()->GetTickResolution() );
        second_keys.Add( time.AsSeconds() );
    }

    return second_keys;
}
