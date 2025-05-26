// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

    // Try to find the closest metakey of iTime
    // If 2 metakeys are close enough (inside the same tolerance range), both can be added to new metachannel
    // But at this time, only 1 metakey must be available in the new metachannel
    // Because this function is only used to get metakey under the mouse
    // And at this time, GUI doesn't manage metakey selection, so multiple check(metakey==1) are used
    TOptional<TPair<FFrameNumber, FMetaKey>> closest_pair;
    for( const auto& pair : mMetaKeys )
    {
        FFrameNumber frame = pair.Key;
        FMetaKey meta_key = pair.Value;

        if( range.Contains( frame ) )
        {
            if( !closest_pair )
            {
                closest_pair = pair;
            }
            else
            {
                if( FMath::Abs( ( closest_pair->Key - iTime ).AsDecimal() ) > FMath::Abs( ( frame - iTime ).AsDecimal() ) )
                {
                    closest_pair = pair;
                }
            }
        }
    }

    if( closest_pair )
        ioMetaChannel->mMetaKeys.Add( *closest_pair );

    for( auto& pair : ioMetaChannel->mMetaKeys )
    {
        FFrameNumber meta_frame = pair.Key;
        FMetaKey& meta_key = pair.Value;

        for( auto& sub_key : meta_key.mSubKeys )
        {
            FFrameNumber key_frame;
            sub_key.mChannelHandle.Get()->GetKeyTime( sub_key.mKeyHandle, key_frame );
            // positive offset = offset to left
            // negative offset = offset to right
            sub_key.mOffset = iTime - key_frame;
        }
    }
}

//---

FFrameTime
FMetaChannel::Move( const FFrameTime& iTime, bool iSnap, const FFrameRate& iTickResolution, const FFrameRate& iDisplayRate, TOptional<TRange<FFrameNumber>> iTrueRangeToClamp )
{
    FFrameNumber last_inner_sub_key_frame; // This is the last sub key in the last meta key moved (at this moment, there is always only one), mainly to set the current frame in the sequencer

    FFrameTime clamped_time = iTime;

    FFrameTime min_offset = TNumericLimits<FFrameNumber>::Max();
    FFrameTime max_offset = TNumericLimits<FFrameNumber>::Min();
    for( auto& pair : mMetaKeys )
    {
        for( auto& sub_key : pair.Value.mSubKeys )
        {
            //UE_LOG( LogTemp, Warning, TEXT( "offset: %d %f" ), sub_key.mOffset.GetFrame().Value, sub_key.mOffset.GetSubFrame() );
            if( min_offset > sub_key.mOffset )
                min_offset = sub_key.mOffset;
            if( max_offset < sub_key.mOffset )
                max_offset = sub_key.mOffset;
        }
    }

    //UE_LOG( LogTemp, Warning, TEXT( "min offset: %d %f" ), min_offset.GetFrame().Value, min_offset.GetSubFrame() );
    //UE_LOG( LogTemp, Warning, TEXT( "max offset: %d %f" ), max_offset.GetFrame().Value, max_offset.GetSubFrame() );

    if( iTrueRangeToClamp.IsSet() && !iTrueRangeToClamp->IsEmpty() )
    {
        //UE_LOG( LogTemp, Warning, TEXT( "iTrueRangeToClamp: [%d %d[" ), iTrueRangeToClamp->GetLowerBoundValue().Value, iTrueRangeToClamp->GetUpperBoundValue().Value );

        FFrameNumber lower_value = ( iTrueRangeToClamp->GetLowerBoundValue() + max_offset ).CeilToFrame();
        FFrameNumber upper_value = ( iTrueRangeToClamp->GetUpperBoundValue() + min_offset ).CeilToFrame(); // Is it really correct ?
        //FFrameNumber upper_value = ( iTrueRangeToClamp->GetUpperBoundValue() + min_offset ).FloorToFrame();
        TRange<FFrameNumber> new_clamped_true_range = TRange<FFrameNumber>( lower_value, upper_value );

        //UE_LOG( LogTemp, Warning, TEXT( "new_clamped_true_range: [%d %d[" ), new_clamped_true_range.GetLowerBoundValue().Value, new_clamped_true_range.GetUpperBoundValue().Value );

        //UE_LOG( LogTemp, Warning, TEXT( "iTime: %d %f" ), clamped_time.GetFrame().Value, clamped_time.GetSubFrame() );
        clamped_time = UE::MovieScene::ClampToDiscreteRange( clamped_time, new_clamped_true_range );
        //UE_LOG( LogTemp, Warning, TEXT( "clamped_time: %d %f" ), clamped_time.GetFrame().Value, clamped_time.GetSubFrame() );
    }

#if 1
    struct FKeyHandleAndFrame
    {
        FKeyHandle KeyHandle;
        //FFrameNumber OldFrame;
        FFrameNumber NewFrame;
    };
    TMultiMap<FMovieSceneChannel*, FKeyHandleAndFrame> new_map;
    for( auto& pair : mMetaKeys )
    {
        for( auto& sub_key : pair.Value.mSubKeys )
        {
            FMovieSceneChannel* channel = sub_key.mChannelHandle.Get();
            if( !channel )
                continue;

            FFrameTime inner_moved_key_time = clamped_time - sub_key.mOffset;
            //FFrameTime inner_moved_key_frame = iTime - sub_key.mOffset;

            //UE_LOG( LogTemp, Warning, TEXT( "inner_moved_key_time: %d %f" ), inner_moved_key_time.FloorToFrame().Value, inner_moved_key_time.GetSubFrame() );

            // From ...\Source\Editor\Sequencer\Private\Tools\EditToolDragOperations.cpp -> OnDrag() -> SnapToInterval()
            if( iSnap )
            {
                //// Convert from resolution to DisplayRate, round to frame, then back again. We floor to frames when using the frame block scrubber, and round using the vanilla scrubber
                //FFrameTime   DisplayTime = FFrameRate::TransformTime( inner_moved_key_time, iTickResolution, iDisplayRate );
                ////FFrameNumber PlayIntervalTime = ScrubStyle == ESequencerScrubberStyle::FrameBlock ? DisplayTime.FloorToFrame() : DisplayTime.RoundToFrame();
                //FFrameNumber PlayIntervalTime = DisplayTime.FloorToFrame();
                //inner_moved_key_time = FFrameRate::TransformTime( PlayIntervalTime, iDisplayRate, iTickResolution ).FloorToFrame();

                // Simple syntax (== above) as we don't manage ScrubStyle
                inner_moved_key_time = FFrameRate::TransformTime( FFrameRate::TransformTime( inner_moved_key_time, iTickResolution, iDisplayRate ).FloorToFrame(), iDisplayRate, iTickResolution );

                //UE_LOG( LogTemp, Warning, TEXT( "inner_moved_key_time (snap): %d %f" ), inner_moved_key_time.FloorToFrame().Value, inner_moved_key_time.GetSubFrame() );
            }

            // Must be done here, because if iTime is clamped outside this function, the mOffset wont be sync'ed to the real value of the subkey
            if( iTrueRangeToClamp.IsSet() )
            {
                if( iTrueRangeToClamp->IsEmpty() )
                    continue;

                inner_moved_key_time = UE::MovieScene::ClampToDiscreteRange( inner_moved_key_time, *iTrueRangeToClamp );
                //UE_LOG( LogTemp, Warning, TEXT( "inner_moved_key_time (clamp): %d %f" ), inner_moved_key_time.FloorToFrame().Value, inner_moved_key_time.GetSubFrame() );

                check( iTrueRangeToClamp->Contains( inner_moved_key_time.FloorToFrame() ) );

                //if( iTrueRangeToClamp->HasLowerBound() && iTrueRangeToClamp->HasUpperBound() )
                //    inner_moved_key_time = FMath::Clamp( inner_moved_key_time, FFrameTime( iTrueRangeToClamp->GetLowerBoundValue() ), FFrameTime( iTrueRangeToClamp->GetUpperBoundValue() - 1 ) ); // -1 because clamp is both inclusive
                //else if( iTrueRangeToClamp->HasLowerBound() )
                //    inner_moved_key_time = FMath::Max( inner_moved_key_time, FFrameTime( iTrueRangeToClamp->GetLowerBoundValue() ) );
                //else if( iTrueRangeToClamp->HasUpperBound() )
                //    inner_moved_key_time = FMath::Min( inner_moved_key_time, FFrameTime( iTrueRangeToClamp->GetUpperBoundValue() - 1 ) );
            }

            last_inner_sub_key_frame = inner_moved_key_time.FloorToFrame();
            //UE_LOG( LogTemp, Warning, TEXT( "last_inner_sub_key_frame: %d" ), last_inner_sub_key_frame.Value );

            //channel->SetKeyTime( sub_key.mKeyHandle, last_inner_sub_key_frame );

            FKeyHandleAndFrame key_and_frame;
            key_and_frame.KeyHandle = sub_key.mKeyHandle;
            key_and_frame.NewFrame = last_inner_sub_key_frame;
            //channel->GetKeyTime( sub_key.mKeyHandle, key_and_frame.OldFrame );

            new_map.Add( channel, key_and_frame );
        }
    }

    TArray<FMovieSceneChannel*> channels;
    new_map.GetKeys( channels );
    for( FMovieSceneChannel* channel : channels )
    {
        TArray<FKeyHandleAndFrame> key_and_frames;
        new_map.MultiFind( channel, key_and_frames );

        TArray<FKeyHandle> key_handles;
        //TArray<FFrameNumber> old_frames;
        TArray<FFrameNumber> new_frames;
        for( const FKeyHandleAndFrame& key_and_frame : key_and_frames )
        {
            key_handles.Add( key_and_frame.KeyHandle );
            //old_frames.Add( key_and_frame.OldFrame );
            new_frames.Add( key_and_frame.NewFrame );
        }

        channel->SetKeyTimes( key_handles, new_frames );
    }

#else

    for( auto& pair : mMetaKeys )
    {
        for( auto& sub_key : pair.Value.mSubKeys )
        {
            FMovieSceneChannel* channel = sub_key.mChannelHandle.Get();
            if( !channel )
                continue;

            FFrameTime inner_moved_key_time = clamped_time - sub_key.mOffset;
            //FFrameTime inner_moved_key_frame = iTime - sub_key.mOffset;

            //UE_LOG( LogTemp, Warning, TEXT( "inner_moved_key_time: %d %f" ), inner_moved_key_time.FloorToFrame().Value, inner_moved_key_time.GetSubFrame() );

            // From ...\Source\Editor\Sequencer\Private\Tools\EditToolDragOperations.cpp -> OnDrag() -> SnapToInterval()
            if( iSnap )
            {
                //// Convert from resolution to DisplayRate, round to frame, then back again. We floor to frames when using the frame block scrubber, and round using the vanilla scrubber
                //FFrameTime   DisplayTime = FFrameRate::TransformTime( inner_moved_key_time, iTickResolution, iDisplayRate );
                ////FFrameNumber PlayIntervalTime = ScrubStyle == ESequencerScrubberStyle::FrameBlock ? DisplayTime.FloorToFrame() : DisplayTime.RoundToFrame();
                //FFrameNumber PlayIntervalTime = DisplayTime.FloorToFrame();
                //inner_moved_key_time = FFrameRate::TransformTime( PlayIntervalTime, iDisplayRate, iTickResolution ).FloorToFrame();

                // Simple syntax (== above) as we don't manage ScrubStyle
                inner_moved_key_time = FFrameRate::TransformTime( FFrameRate::TransformTime( inner_moved_key_time, iTickResolution, iDisplayRate ).FloorToFrame(), iDisplayRate, iTickResolution );

                //UE_LOG( LogTemp, Warning, TEXT( "inner_moved_key_time (snap): %d %f" ), inner_moved_key_time.FloorToFrame().Value, inner_moved_key_time.GetSubFrame() );
            }

            // Must be done here, because if iTime is clamped outside this function, the mOffset wont be sync'ed to the real value of the subkey
            if( iTrueRangeToClamp.IsSet() )
            {
                if( iTrueRangeToClamp->IsEmpty() )
                    continue;

                inner_moved_key_time = UE::MovieScene::ClampToDiscreteRange( inner_moved_key_time, *iTrueRangeToClamp );
                //UE_LOG( LogTemp, Warning, TEXT( "inner_moved_key_time (clamp): %d %f" ), inner_moved_key_time.FloorToFrame().Value, inner_moved_key_time.GetSubFrame() );

                check( iTrueRangeToClamp->Contains( inner_moved_key_time.FloorToFrame() ) );

                //if( iTrueRangeToClamp->HasLowerBound() && iTrueRangeToClamp->HasUpperBound() )
                //    inner_moved_key_time = FMath::Clamp( inner_moved_key_time, FFrameTime( iTrueRangeToClamp->GetLowerBoundValue() ), FFrameTime( iTrueRangeToClamp->GetUpperBoundValue() - 1 ) ); // -1 because clamp is both inclusive
                //else if( iTrueRangeToClamp->HasLowerBound() )
                //    inner_moved_key_time = FMath::Max( inner_moved_key_time, FFrameTime( iTrueRangeToClamp->GetLowerBoundValue() ) );
                //else if( iTrueRangeToClamp->HasUpperBound() )
                //    inner_moved_key_time = FMath::Min( inner_moved_key_time, FFrameTime( iTrueRangeToClamp->GetUpperBoundValue() - 1 ) );
            }

            last_inner_sub_key_frame = inner_moved_key_time.FloorToFrame();
            //UE_LOG( LogTemp, Warning, TEXT( "last_inner_sub_key_frame: %d" ), last_inner_sub_key_frame.Value );

            channel->SetKeyTime( sub_key.mKeyHandle, last_inner_sub_key_frame );
        }
    }

#endif

    return last_inner_sub_key_frame;
}
