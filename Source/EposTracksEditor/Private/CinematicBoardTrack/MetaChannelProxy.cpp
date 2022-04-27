// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "CinematicBoardTrack/MetaChannelProxy.h"

#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "ISequencerChannelInterface.h"
#include "ISequencerModule.h"

#include "KeyThumbnail/KeyThumbnailSection.h"

//---

FMetaChannel::FMetaChannel( const FFrameNumber& iMergeTolerance )
    : mMergeTolerance( iMergeTolerance )
{
}

FMetaChannel::~FMetaChannel()
{
}

int32
FMetaChannel::NumMetaKeys() const
{
    return mMetaKeys.Num();
}

const TMap<FFrameNumber, FMetaKey>&
FMetaChannel::GetMetaKeys() const
{
    return mMetaKeys;
}

TSharedPtr<FMetaChannel>
FMetaChannel::CreateFromTime( const FFrameTime& iTime, const FFrameNumber& iTolerance ) const
{
    TSharedPtr<FMetaChannel> new_meta_channel = MakeShared<FMetaChannel>( mMergeTolerance );

    FillWithTime( iTime, iTolerance, new_meta_channel );

    return new_meta_channel;
}

//---

void
FMetaChannel::Build( const FChannelProxyBySectionMap& iChannelProxyMap )
{
    BuildSubKeys( iChannelProxyMap );
    BuildDrawKeys();
    BuildFlags();
}

void
FMetaChannel::BuildSubKeys( const FChannelProxyBySectionMap& iChannelProxyMap )
{
    if( !iChannelProxyMap.Num() )
        return;

    TMap<FFrameNumber, FMetaKey> meta_keys;

    for( const auto& pair : iChannelProxyMap )
    {
        TSharedPtr<FMovieSceneChannelProxy> channel_proxy = pair.Value;
        TWeakObjectPtr<UMovieSceneSection> section = pair.Key;

        TArrayView<const FMovieSceneChannelEntry> entries = channel_proxy->GetAllEntries();
        check( entries.Num() <= 1 ); // Should always be the case, as only 1 type of channel should be in the proxy (transform or material or ...)
        for( const auto& entry : entries )
        {
            TArrayView<FMovieSceneChannel* const> channels = entry.GetChannels();
            for( int channel_index = 0; channel_index < channels.Num(); channel_index++ )
            {
                auto channel = channels[channel_index];
                FMovieSceneChannelHandle channel_handle = channel_proxy->MakeHandle( entry.GetChannelTypeName(), channel_index );

                TArray<FFrameNumber> key_times;
                TArray<FKeyHandle> key_handles;
                channel->GetKeys( TRange<FFrameNumber>::All(), &key_times, &key_handles );
                check( key_times.Num() == key_handles.Num() );

                for( int key_index = 0; key_index < key_times.Num(); key_index++ )
                {
                    // Create the new sub key corresponding to the current key
                    FMetaKey::FSubKey sub_key;
                    sub_key.mChannelHandle = channel_handle;
                    sub_key.mKeyHandle = key_handles[key_index];
                    sub_key.mSection = section;

                    FFrameNumber time = key_times[key_index];

                    // Check if the current sub key time is already near an existing meta key
                    bool found_key = false;
                    FFrameNumber key_to_add;
                    for( const auto& pair2 : meta_keys )
                    {
                        TRange<FFrameNumber> range_tolerance( pair2.Key - mMergeTolerance, pair2.Key + 1 + mMergeTolerance );

                        if( range_tolerance.Contains( time ) )
                        {
                            found_key = true;
                            key_to_add = pair2.Key;
                            break;
                        }
                    }

                    // If the current sub key time is near an existing meta key
                    if( found_key )
                    {
                        FMetaKey* meta_key = meta_keys.Find( key_to_add );
                        check( meta_key );

                        meta_key->mSubKeys.Add( sub_key );
                    }
                    // Otherwise, just add a new meta key with the new sub key
                    else
                    {
                        FMetaKey& meta_key = meta_keys.FindOrAdd( time );

                        meta_key.mSubKeys.Add( sub_key );
                        meta_key.mFlags = FMetaKey::EFlags::kNone;
                    }
                }
            }
        }
    }

    check( !mMetaKeys.Num() );

    for( const auto& pair : meta_keys )
    {
        FMetaKey meta_key = pair.Value;
        FFrameNumber sum = 0;
        for( auto sub_key : meta_key.mSubKeys )
        {
            FFrameNumber time;
            sub_key.mChannelHandle.Get()->GetKeyTime( sub_key.mKeyHandle, time );
            sum += time;
        }
        FFrameNumber average_time = sum / meta_key.mSubKeys.Num();

        mMetaKeys.Add( average_time, meta_key );
    }
}

ISequencerChannelInterface*
FMetaChannel::FindChannelEditorInterface( FMovieSceneChannelHandle iChannelHandle ) const
{
    ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" );
    ISequencerChannelInterface* EditorInterface = SequencerModule.FindChannelEditorInterface( iChannelHandle.GetChannelTypeName() );
    //ensureMsgf( EditorInterface, TEXT( "No channel interface found for type '%s'. Did you forget to call ISequencerModule::RegisterChannelInterface<ChannelType>()?" ), *ChannelHandle.GetChannelTypeName().ToString() );
    return EditorInterface;
}

ISequencerChannelInterface*
FMetaChannel::FindFirstChannelEditorInterface() const
{
    for( const auto& pair : mMetaKeys )
    {
        const FMetaKey& meta_key = pair.Value;

        for( const auto& sub_key : meta_key.mSubKeys )
        {
            ISequencerChannelInterface* editorInterface = FindChannelEditorInterface( sub_key.mChannelHandle );
            if( editorInterface )
                return editorInterface;
        }
    }

    return nullptr;
}

void
FMetaChannel::BuildDrawKeys()
{
    ISequencerChannelInterface* editorInterface = FindFirstChannelEditorInterface();
    if( !editorInterface )
        return;

    for( auto& pair : mMetaKeys )
    {
        FMetaKey& meta_key = pair.Value;

        for( auto& sub_key : meta_key.mSubKeys )
        {
            check( sub_key.mChannelHandle.Get() );

            editorInterface->DrawKeys_Raw( sub_key.mChannelHandle.Get(), MakeArrayView( &sub_key.mKeyHandle, 1 ), sub_key.mSection.Get(), MakeArrayView( &sub_key.mKeyDrawParam, 1 ) );
        }
    }
}

void
FMetaChannel::BuildFlags()
{
    for( auto& pair : mMetaKeys )
    {
        FFrameNumber time = pair.Key;
        FMetaKey& meta_key = pair.Value;

        const TArray<FMetaKey::FSubKey>& sub_keys = meta_key.mSubKeys;

        check( sub_keys.Num() );
        meta_key.mMetaKeyDrawParam = sub_keys[0].mKeyDrawParam; // Init

        for( int i = 1; i < sub_keys.Num(); i++ )
        {
            if( sub_keys[i - 1].mKeyDrawParam != sub_keys[i].mKeyDrawParam )
            {
                meta_key.mFlags = FMetaKey::EFlags::kPartial;

                static const FSlateBrush* PartialKeyBrush = FAppStyle::Get().GetBrush( "Sequencer.PartialKey" );
                meta_key.mMetaKeyDrawParam.BorderBrush = PartialKeyBrush;
                meta_key.mMetaKeyDrawParam.FillBrush = PartialKeyBrush;
                meta_key.mMetaKeyDrawParam.FillOffset = FVector2D( 0.f, 0.f );
                meta_key.mMetaKeyDrawParam.FillTint = FLinearColor::White;
                meta_key.mMetaKeyDrawParam.BorderTint = FLinearColor::White;

                break;
            }
        }
    }
}

//---

void
FMetaChannel::FillWithTime( const FFrameTime& iTime, const FFrameNumber& iTolerance, TSharedPtr<FMetaChannel> ioMetaChannel ) const
{
    TRange<FFrameNumber> range( ( iTime - iTolerance ).GetFrame(), ( iTime + iTolerance ).GetFrame() + 1 );

    for( const auto& pair : mMetaKeys )
    {
        if( range.Contains( pair.Key ) )
            ioMetaChannel->mMetaKeys.Add( pair.Key, pair.Value );
    }

    for( auto& pair : ioMetaChannel->mMetaKeys )
    {
        FFrameNumber time = pair.Key;
        FMetaKey& meta_key = pair.Value;

        for( auto& sub_key : meta_key.mSubKeys )
        {
            FFrameNumber key_time;
            sub_key.mChannelHandle.Get()->GetKeyTime( sub_key.mKeyHandle, key_time );
            sub_key.mOffset = iTime - key_time;
        }
    }
}

//---

FFrameTime
FMetaChannel::Move( const FFrameTime& iTime, bool iSnap, const FFrameRate& iTickResolution, const FFrameRate& iDisplayRate )
{
    FFrameNumber last_inner_sub_key; // This is the last sub key in the last meta key moved (at this moment, there is always only one), mainly to set the current frame in the sequencer

    for( auto& pair : mMetaKeys )
    {
        for( auto& sub_key : pair.Value.mSubKeys )
        {
            FMovieSceneChannel* channel = sub_key.mChannelHandle.Get();
            if( !channel )
                continue;

            FFrameTime inner_moved_key_frame = iTime - sub_key.mOffset;

            // From ...\Source\Editor\Sequencer\Private\Tools\EditToolDragOperations.cpp -> OnDrag() -> SnapToInterval()
            if( iSnap )
            {
                //// Convert from resolution to DisplayRate, round to frame, then back again. We floor to frames when using the frame block scrubber, and round using the vanilla scrubber
                //FFrameTime   DisplayTime = FFrameRate::TransformTime( inner_moved_key_frame, iTickResolution, iDisplayRate );
                ////FFrameNumber PlayIntervalTime = ScrubStyle == ESequencerScrubberStyle::FrameBlock ? DisplayTime.FloorToFrame() : DisplayTime.RoundToFrame();
                //FFrameNumber PlayIntervalTime = DisplayTime.FloorToFrame();
                //inner_moved_key_frame = FFrameRate::TransformTime( PlayIntervalTime, iDisplayRate, iTickResolution ).FloorToFrame();

                // Simple syntax (== above) as we don't manage ScrubStyle
                inner_moved_key_frame = FFrameRate::TransformTime( FFrameRate::TransformTime( inner_moved_key_frame, iTickResolution, iDisplayRate ).FloorToFrame(), iDisplayRate, iTickResolution );
            }

            last_inner_sub_key = inner_moved_key_frame.GetFrame();

            channel->SetKeyTime( sub_key.mKeyHandle, last_inner_sub_key );
        }
    }

    return last_inner_sub_key;
}
