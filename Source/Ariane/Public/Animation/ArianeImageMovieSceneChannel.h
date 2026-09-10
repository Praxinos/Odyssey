// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal Headers
#include "CoreMinimal.h"
#include "MovieSceneSection.h"
#include "Channels/MovieSceneEventChannel.h"
#include "SequencerChannelTraits.h"
#include "ScopedTransaction.h"
#include "Channels/MovieSceneChannelData.h"
// Ariane Headers
#include "ArianeImageKeyData.h"
#include "ArianeImage.h"

#include "ArianeImageMovieSceneChannel.generated.h"

USTRUCT()
struct ARIANE_API FArianeImageMovieSceneChannel : public FMovieSceneChannel
{
    GENERATED_BODY()

    using CurveValueType = FArianeImageKeyData;

    TMovieSceneChannelData<FArianeImageKeyData> GetData()
    {
        return TMovieSceneChannelData<FArianeImageKeyData>(&Times, &Values, this, &KeyHandles);
    }

    TMovieSceneChannelData<const FArianeImageKeyData> GetData() const
    {
        return TMovieSceneChannelData<const FArianeImageKeyData>(&Times, &Values);
    }

    // Gets the key <= time
    bool Evaluate(FFrameTime InTime, FArianeImageKeyData& OutValue) const;

    // Implements FMovieSceneChannel::GetKeys
    virtual void GetKeys(const TRange<FFrameNumber>& WithinRange, TArray<FFrameNumber>* OutKeyTimes, TArray<FKeyHandle>* OutKeyHandles) override;
    // Implements FMovieSceneChannel::GetKeyTimes
    virtual void GetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<FFrameNumber> OutKeyTimes) override;
    // Implements FMovieSceneChannel::SetKeyTimes
    virtual void SetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<const FFrameNumber> InKeyTimes) override;
    // Implements FMovieSceneChannel::DuplicateKeys
    virtual void DuplicateKeys(TArrayView<const FKeyHandle> InHandles, TArrayView<FKeyHandle> OutNewHandles) override;
    // Implements FMovieSceneChannel::DeleteKeys
    virtual void DeleteKeys(TArrayView<const FKeyHandle> InHandles) override;
    // Implements FMovieSceneChannel::DeleteKeysFrom
    virtual void DeleteKeysFrom(FFrameNumber InTime, bool bDeleteKeysBefore) override;
    // Implements FMovieSceneChannel::ChangeFrameResolution
    virtual void ChangeFrameResolution(FFrameRate SourceRate, FFrameRate DestinationRate) override;
    // Implements FMovieSceneChannel::ComputeEffectiveRange
    virtual TRange<FFrameNumber> ComputeEffectiveRange() const override;
    // Implements FMovieSceneChannel::GetNumKeys
    virtual int32 GetNumKeys() const override;
    // Implements FMovieSceneChannel::Reset
    virtual void Reset() override;
    // Implements FMovieSceneChannel::Offset
    virtual void Offset(FFrameNumber DeltaPosition) override;

    void SetDefault(FArianeImageKeyData InDefault) { DefaultValue = MoveTemp(InDefault); }
    TOptional<FArianeImageKeyData> GetDefault() const { return DefaultValue; }
    void RemoveDefault() { DefaultValue.Reset(); }
    bool HasDefault() const { return DefaultValue.IsSet(); }

    void PostLoad();
    void PostEditUndo();

private:
    UPROPERTY(meta = (KeyTimes))
    TArray<FFrameNumber> Times;

    UPROPERTY(meta = (KeyValues))
    TArray<FArianeImageKeyData> Values;

    FMovieSceneKeyHandleMap KeyHandles;

private:
    TOptional<FArianeImageKeyData> DefaultValue;
};
