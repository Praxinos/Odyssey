// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Channels/MovieSceneChannelHandle.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Sections/ThumbnailSection.h"
#include "TrackEditors/SubTrackEditorBase.h"
#include "KeyDrawParams.h"

#include "KeyThumbnail/KeyThumbnailSection.h"

//---

struct FMetaKey
{
    enum class EFlags
    {
        kNone,
        kPartial    = 1 << 0,
    };

    struct FSubKey
    {
        FMovieSceneChannelHandle            mChannelHandle;
        FKeyHandle                          mKeyHandle;
        TWeakObjectPtr<UMovieSceneSection>  mSection;
        FKeyDrawParams              mKeyDrawParam;  // The 'real' draw params of the sub key
        FFrameTime                  mOffset;        // The offset of each sub key to a reference frame (when a click is done in the meta channel for example, and used when moving sub keys, because all sub keys of the same meta key don't may have the same time)
    };

    TArray<FSubKey>         mSubKeys;
    EFlags                  mFlags;
    FKeyDrawParams          mMetaKeyDrawParam; // The draw param which will be used to draw inside the meta channel
};

//---

template<typename ChannelType, typename ValueType>
struct TMetaChannel
{
public:
    TMetaChannel( const FFrameNumber& iMergeTolerance );
    virtual ~TMetaChannel();

    /** Build the meta channel from a channel proxy depending of the ChannelType */
    virtual void Build( const FChannelProxyBySectionMap& iChannelProxyMap );

    /** Move all sub keys to the new time */
    virtual FFrameTime Move( const FFrameTime& iTime, bool iSnap, const FFrameRate& iTickResolution, const FFrameRate& iDisplayRate );

public:
    /** Get the number of meta key */
    int32 NumMetaKeys() const;

    /** Get all meta keys (as const) */
    const TMap<FFrameNumber, FMetaKey>& GetMetaKeys() const;

protected:
    /** Build all the sub keys */
    virtual void BuildSubKeys( const FChannelProxyBySectionMap& iChannelProxyMap );
    /** Build all the FKeyDrawParams of all sub keys */
    virtual void BuildDrawKeys() = 0;
    /** Set flags of all meta keys */
    virtual void BuildFlags();

    /** Get all meta keys at the given time of the current meta channel and copy them inside the new (empty) meta channel
        This function must be used inside CreateFromTime(...) which returns the 'real' meta channel type
    */
    virtual void FillWithTime( const FFrameTime& iTime, const FFrameNumber& iTolerance, TSharedPtr<TMetaChannel<ChannelType, ValueType>> ioMetaChannel );

protected:
    FFrameNumber                    mMergeTolerance;
    TMap<FFrameNumber, FMetaKey>    mMetaKeys;
};

//---

struct FMetaFloatChannel
    : TMetaChannel<FMovieSceneFloatChannel, FMovieSceneFloatValue>
{
public:
    FMetaFloatChannel( const FFrameNumber& iMergeTolerance );

public:
    /** Create a new meta channel with all meta keys at the given time of the current meta channel */
    virtual TSharedPtr<FMetaFloatChannel> CreateFromTime( const FFrameTime& iTime, const FFrameNumber& iTolerance );

protected:
    /** Build all the FKeyDrawParams of all sub keys */
    virtual void BuildDrawKeys();
};

//---

struct FMetaMaterialChannel
    : TMetaChannel<FMovieSceneObjectPathChannel, FMovieSceneObjectPathChannelKeyValue>
{
public:
    FMetaMaterialChannel( const FFrameNumber& iMergeTolerance );

public:
    /** Create a new meta channel with all meta keys at the given time of the current meta channel */
    virtual TSharedPtr<FMetaMaterialChannel> CreateFromTime( const FFrameTime& iTime, const FFrameNumber& iTolerance );

protected:
    /** Build all the FKeyDrawParams of all sub keys */
    virtual void BuildDrawKeys();
};

//---
//---
//---

template<typename ChannelType, typename ValueType>
TMetaChannel<ChannelType, ValueType>::TMetaChannel( const FFrameNumber& iMergeTolerance )
    : mMergeTolerance( iMergeTolerance )
{
}

template<typename ChannelType, typename ValueType>
TMetaChannel<ChannelType, ValueType>::~TMetaChannel()
{
}

template<typename ChannelType, typename ValueType>
int32
TMetaChannel<ChannelType, ValueType>::NumMetaKeys() const
{
    return mMetaKeys.Num();
}

template<typename ChannelType, typename ValueType>
const TMap<FFrameNumber, FMetaKey>&
TMetaChannel<ChannelType, ValueType>::GetMetaKeys() const
{
    return mMetaKeys;
}

//---

template<typename ChannelType, typename ValueType>
void
TMetaChannel<ChannelType, ValueType>::Build( const FChannelProxyBySectionMap& iChannelProxyMap )
{
    BuildSubKeys( iChannelProxyMap );
    BuildDrawKeys();
    BuildFlags();
}

template<typename ChannelType, typename ValueType>
void
TMetaChannel<ChannelType, ValueType>::BuildSubKeys( const FChannelProxyBySectionMap& iChannelProxyMap )
{
    if( !iChannelProxyMap.Num() )
        return;

    TMap<FFrameNumber, FMetaKey> meta_keys;

    for( const auto& pair : iChannelProxyMap )
    {
        TSharedPtr<FMovieSceneChannelProxy> channel_proxy = pair.Value;

        TArrayView<ChannelType*> channels = channel_proxy->GetChannels<ChannelType>();

        for( int32 channel_index = 0; channel_index < channels.Num(); ++channel_index )
        {
            TMovieSceneChannelHandle<ChannelType> channel_handle = channel_proxy->MakeHandle<ChannelType>( channel_index );

            ChannelType* channel = channel_handle.Get();
            if( !channel || !channel->GetNumKeys() )
                continue;

            TMovieSceneChannelData<ValueType> channel_data = channel->GetData();

            for( int32 key_index = 0; key_index < channel->GetNumKeys(); ++key_index )
            {
                FKeyHandle key_handle = channel_data.GetHandle( key_index );

                // Create the new sub key corresponding to the current key
                FMetaKey::FSubKey sub_key;
                sub_key.mChannelHandle = channel_handle;
                sub_key.mKeyHandle = key_handle;
                sub_key.mSection = pair.Key;

                FFrameNumber time = channel_data.GetTimes()[key_index];

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

template<typename ChannelType, typename ValueType>
void
TMetaChannel<ChannelType, ValueType>::BuildFlags()
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

                static const FSlateBrush* PartialKeyBrush = FEditorStyle::GetBrush( "Sequencer.PartialKey" );
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

template<typename ChannelType, typename ValueType>
void
TMetaChannel<ChannelType, ValueType>::FillWithTime( const FFrameTime& iTime, const FFrameNumber& iTolerance, TSharedPtr<TMetaChannel<ChannelType, ValueType>> ioMetaChannel )
{
    TRange<FFrameNumber> range( ( iTime - iTolerance ).GetFrame(), ( iTime + iTolerance ).GetFrame() + 1 );

    for( auto& pair : mMetaKeys )
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

template<typename ChannelType, typename ValueType>
FFrameTime
TMetaChannel<ChannelType, ValueType>::Move( const FFrameTime& iTime, bool iSnap, const FFrameRate& iTickResolution, const FFrameRate& iDisplayRate )
{
    FFrameNumber last_inner_sub_key; // This is the last sub key in the last meta key moved (at this moment, there is always only one), mainly to set the current frame in the sequencer

    for( auto& pair : mMetaKeys )
    {
        for( auto& sub_key : pair.Value.mSubKeys )
        {
            TMovieSceneChannelHandle<ChannelType> channel_handle = sub_key.mChannelHandle.template Cast<ChannelType>(); // https://stackoverflow.com/questions/3786360/confusing-template-error#answer-3786481
            FKeyHandle& key_handle = sub_key.mKeyHandle;
            FFrameTime offset = sub_key.mOffset;

            ChannelType* channel = channel_handle.Get();
            if( !channel )
                continue;

            TMovieSceneChannelData<ValueType> channel_data = channel->GetData();
            int32 key_index = channel_data.GetIndex( key_handle );

            FFrameTime inner_moved_key_frame = iTime - offset;

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
            int32 new_key_index = channel_data.MoveKey( key_index, last_inner_sub_key );
            key_handle = channel_data.GetHandle( new_key_index );
        }
    }

    return last_inner_sub_key;
}
