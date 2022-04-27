// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Channels/MovieSceneChannelHandle.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Sections/ThumbnailSection.h"
#include "TrackEditors/SubTrackEditorBase.h"
#include "KeyDrawParams.h"

#include "EposSequenceHelpers.h"

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

struct ISequencerChannelInterface;

class FMetaChannel
{
public:
    FMetaChannel( const FFrameNumber& iMergeTolerance );
    virtual ~FMetaChannel();

    /** Build the meta channel from a channel proxy depending of the ChannelType */
    virtual void Build( const FChannelProxyBySectionMap& iChannelProxyMap );

    /** Move all sub keys to the new time */
    virtual FFrameTime Move( const FFrameTime& iTime, bool iSnap, const FFrameRate& iTickResolution, const FFrameRate& iDisplayRate );

public:
    /** Get the number of meta key */
    int32 NumMetaKeys() const;

    /** Get all meta keys (as const) */
    const TMap<FFrameNumber, FMetaKey>& GetMetaKeys() const;

public:
    /** Create a new meta channel with all meta keys at the given time of the current meta channel */
    virtual TSharedPtr<FMetaChannel> CreateFromTime( const FFrameTime& iTime, const FFrameNumber& iTolerance ) const;

protected:
    /** Build all the sub keys */
    virtual void BuildSubKeys( const FChannelProxyBySectionMap& iChannelProxyMap );
    /** Build all the FKeyDrawParams of all sub keys */
    virtual void BuildDrawKeys();
    /** Set flags of all meta keys */
    virtual void BuildFlags();

    /** Get all meta keys at the given time of the current meta channel and copy them inside the new (empty) meta channel
        This function must be used inside CreateFromTime(...) which returns the 'real' meta channel type
    */
    virtual void FillWithTime( const FFrameTime& iTime, const FFrameNumber& iTolerance, TSharedPtr<FMetaChannel> ioMetaChannel ) const;

    /** Find the first sequencer channel interface for subkeys
    *   (Assume every subkeys have the same channel type)
    */
    ISequencerChannelInterface* FindFirstChannelEditorInterface() const;

    ISequencerChannelInterface* FindChannelEditorInterface( FMovieSceneChannelHandle iChannelHandle ) const;

protected:
    FFrameNumber                    mMergeTolerance;
    TMap<FFrameNumber, FMetaKey>    mMetaKeys;
};
