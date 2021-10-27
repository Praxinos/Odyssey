// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardTrack/MetaChannelProxy.h"

#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "ISequencerChannelInterface.h"
#include "ISequencerModule.h"

//---

static
ISequencerChannelInterface*
FindChannelEditorInterface( FMovieSceneChannelHandle iChannelHandle )
{
    ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" );
    ISequencerChannelInterface* EditorInterface = SequencerModule.FindChannelEditorInterface( iChannelHandle.GetChannelTypeName() );
    //ensureMsgf( EditorInterface, TEXT( "No channel interface found for type '%s'. Did you forget to call ISequencerModule::RegisterChannelInterface<ChannelType>()?" ), *ChannelHandle.GetChannelTypeName().ToString() );
    return EditorInterface;
}

//---

FMetaFloatChannel::FMetaFloatChannel( const FFrameNumber& iMergeTolerance )
    : TMetaChannel<FMovieSceneFloatChannel, FMovieSceneFloatValue>( iMergeTolerance )
{
}

TSharedPtr<FMetaFloatChannel>
FMetaFloatChannel::CreateFromTime( const FFrameTime& iTime, const FFrameNumber& iTolerance )
{
    TSharedPtr<FMetaFloatChannel> new_meta_channel = MakeShared<FMetaFloatChannel>( mMergeTolerance );

    FillWithTime( iTime, iTolerance, new_meta_channel );

    return new_meta_channel;
}

//---
//---
//---

FMetaMaterialChannel::FMetaMaterialChannel( const FFrameNumber& iMergeTolerance )
    : TMetaChannel<FMovieSceneObjectPathChannel, FMovieSceneObjectPathChannelKeyValue>( iMergeTolerance )
{
}

TSharedPtr<FMetaMaterialChannel>
FMetaMaterialChannel::CreateFromTime( const FFrameTime& iTime, const FFrameNumber& iTolerance )
{
    TSharedPtr<FMetaMaterialChannel> new_meta_channel = MakeShared<FMetaMaterialChannel>( mMergeTolerance );

    FillWithTime( iTime, iTolerance, new_meta_channel );

    return new_meta_channel;
}
