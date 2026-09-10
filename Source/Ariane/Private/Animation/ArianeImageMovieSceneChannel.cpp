// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Arizne Headers
#include "ArianeImageMovieSceneChannel.h"
#include "ArianeImageMovieSceneSection.h"
#include "ArianeImageMovieSceneTrack.h"
#include "ArianeImage.h"
// Unreal headers
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneChannelData.h"
#include "MovieScene.h"
#include "MovieSceneTrack.h"
#include "ISequencer.h"

bool FArianeImageMovieSceneChannel::Evaluate(FFrameTime InTime, FArianeImageKeyData& OutValue) const
{
    if (Times.Num() == 0)
    {
        return false;
    }

    const int32 Index = Algo::UpperBound(Times, InTime.FrameNumber) - 1;

    if (Index < 0)
    {
        return false;
    }

    // Get the next key
    OutValue = Values[Index];


    return true;
}

void FArianeImageMovieSceneChannel::GetKeys(const TRange<FFrameNumber>& WithinRange, TArray<FFrameNumber>* OutKeyTimes, TArray<FKeyHandle>* OutKeyHandles)
{
    GetData().GetKeys(WithinRange, OutKeyTimes, OutKeyHandles);
}

void FArianeImageMovieSceneChannel::GetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<FFrameNumber> OutKeyTimes)
{
    GetData().GetKeyTimes(InHandles, OutKeyTimes);
}

void FArianeImageMovieSceneChannel::SetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<const FFrameNumber> InKeyTimes)
{
    GetData().SetKeyTimes(InHandles, InKeyTimes);
}

void FArianeImageMovieSceneChannel::DuplicateKeys(TArrayView<const FKeyHandle> InHandles, TArrayView<FKeyHandle> OutNewHandles)
{
    GetData().DuplicateKeys(InHandles, OutNewHandles);
}

void FArianeImageMovieSceneChannel::DeleteKeys(TArrayView<const FKeyHandle> InHandles)
{
    GetData().DeleteKeys(InHandles);
}

void FArianeImageMovieSceneChannel::DeleteKeysFrom(FFrameNumber InTime, bool bDeleteKeysBefore)
{
    GetData().DeleteKeysFrom(InTime, bDeleteKeysBefore);
}

void FArianeImageMovieSceneChannel::ChangeFrameResolution(FFrameRate SourceRate, FFrameRate DestinationRate)
{
    GetData().ChangeFrameResolution(SourceRate, DestinationRate);
}

TRange<FFrameNumber> FArianeImageMovieSceneChannel::ComputeEffectiveRange() const
{
    return GetData().GetTotalRange();
}

int32 FArianeImageMovieSceneChannel::GetNumKeys() const
{
    return Times.Num();
}

void FArianeImageMovieSceneChannel::Reset()
{
    Times.Reset();
    Values.Reset();
    KeyHandles.Reset();
}

void FArianeImageMovieSceneChannel::Offset(FFrameNumber DeltaPosition)
{
    GetData().Offset(DeltaPosition);
}

void
FArianeImageMovieSceneChannel::PostLoad()
{
    for( FArianeImageKeyData& Value : Values )
    {
        Value.PostLoad();
    }
}

void
FArianeImageMovieSceneChannel::PostEditUndo()
{
    for( FArianeImageKeyData& Value : Values )
    {
        Value.PostEditUndo();
    }
}
