// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "NoteTrack/MovieSceneNoteSection.h"
#include "UObject/SequencerObjectVersion.h"
#include "Channels/MovieSceneChannelProxy.h"

#define LOCTEXT_NAMESPACE "MovieSceneNoteSection"

/* UMovieSceneFadeSection structors
 *****************************************************************************/

UMovieSceneNoteSection::UMovieSceneNoteSection()
{
    SetRange( TRange<FFrameNumber>::All() );

    //EvalOptions.EnableAndSetCompletionMode
    //( GetLinkerCustomVersion( FSequencerObjectVersion::GUID ) < FSequencerObjectVersion::WhenFinishedDefaultsToProjectDefault ?
    //  EMovieSceneCompletionMode::RestoreState :
    //  EMovieSceneCompletionMode::ProjectDefault );

    BlendType = EMovieSceneBlendType::Absolute;
    bSupportsInfiniteRange = true;

    FMovieSceneChannelProxyData Channels;

#if WITH_EDITOR

    //ChannelProxy = MakeShared<FMovieSceneChannelProxy>( StringCurve, FMovieSceneChannelMetaData(), TMovieSceneExternalValue<FString>::Make() );
    static FMovieSceneChannelMetaData MetaData( "Text", LOCTEXT( "TextChannelName", "Text" ) );
    MetaData.SortOrder = 0;
    //MetaData.bCanCollapseToTrack = false;
    Channels.Add( StringCurve, MetaData, TMovieSceneExternalValue<FString>::Make() );

    static FMovieSceneChannelMetaData MetaData2( "TestType", LOCTEXT( "TestTypeChannelName", "TestType" )/*, LOCTEXT( "ParametersGroupName", "Parameters" )*/ ); // but the group starts before text channel even with sort order ...
    MetaData2.SortOrder = 1;
    //MetaData2.bCanCollapseToTrack = false;
    Channels.Add( TestCurve, MetaData2, TMovieSceneExternalValue<FString>::Make() );

    static FMovieSceneChannelMetaData MetaData3( "TestOpacity", LOCTEXT( "TestOpacityChannelName", "TestOpacity" )/*, LOCTEXT( "ParametersGroupName", "Parameters" )*/ );
    MetaData3.SortOrder = 2;
    //MetaData3.bCanCollapseToTrack = false;
    Channels.Add( Test2Curve, MetaData3, TMovieSceneExternalValue<float>::Make() );

#else

    ChannelProxy = MakeShared<FMovieSceneChannelProxy>( StringCurve );

#endif

    ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( Channels ) );
}

#undef LOCTEXT_NAMESPACE
