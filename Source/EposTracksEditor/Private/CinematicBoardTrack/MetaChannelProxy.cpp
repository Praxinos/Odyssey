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
FindChannelEditorInterface( FName iChannelTypeName )
{
    ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" );
    ISequencerChannelInterface* EditorInterface = SequencerModule.FindChannelEditorInterface( iChannelTypeName );
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

void
FMetaFloatChannel::BuildDrawKeys()
{
    ISequencerChannelInterface* EditorInterface = FindChannelEditorInterface( FMovieSceneFloatChannel::StaticStruct()->GetFName() );
    //FMovieSceneChannel* Channel = ChannelHandle.Get();
    //UMovieSceneSection* OwningSection = GetOwningSection();

    for( auto& pair : mMetaKeys )
    {
        FMetaKey& meta_key = pair.Value;

        for( auto& sub_key : meta_key.mSubKeys )
        {
            TMovieSceneChannelHandle<FMovieSceneFloatChannel> channel_handle = sub_key.mChannelHandle.Cast<FMovieSceneFloatChannel>();
            FMovieSceneFloatChannel* float_channel = channel_handle.Get();
            if( !float_channel )
                continue;

            EditorInterface->DrawKeys_Raw( float_channel, MakeArrayView( &sub_key.mKeyHandle, 1 ), nullptr, MakeArrayView( &sub_key.mKeyDrawParam, 1 ) );
        }
    }
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

//---

void
FMetaMaterialChannel::BuildDrawKeys()
{
    ISequencerChannelInterface* EditorInterface = FindChannelEditorInterface( FMovieSceneObjectPathChannel::StaticStruct()->GetFName() );
    //FMovieSceneChannel* Channel = ChannelHandle.Get();
    //UMovieSceneSection* OwningSection = GetOwningSection();

    for( auto& pair : mMetaKeys )
    {
        FMetaKey& meta_key = pair.Value;

        for( auto& sub_key : meta_key.mSubKeys )
        {
            TMovieSceneChannelHandle<FMovieSceneObjectPathChannel> channel_handle = sub_key.mChannelHandle.Cast<FMovieSceneObjectPathChannel>();
            FMovieSceneObjectPathChannel* material_channel = channel_handle.Get();
            if( !material_channel )
                continue;

            EditorInterface->DrawKeys_Raw( material_channel, MakeArrayView( &sub_key.mKeyHandle, 1 ), nullptr, MakeArrayView( &sub_key.mKeyDrawParam, 1 ) );
        }
    }
}
