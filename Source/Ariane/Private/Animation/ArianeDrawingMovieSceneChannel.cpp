// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Arizne Headers
#include "ArianeDrawingMovieSceneChannel.h"
#include "ArianeDrawingMovieSceneSection.h"
#include "ArianeDrawingMovieSceneTrack.h"
#include "ArianeDrawing.h"
// Unreal headers
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneChannelData.h"
#include "MovieScene.h"
#include "MovieSceneTrack.h"
#include "ISequencer.h"

bool FArianeDrawingMovieSceneChannel::Evaluate(FFrameTime InTime, FArianeDrawingKeyData& OutValue) const
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

void FArianeDrawingMovieSceneChannel::GetKeys(const TRange<FFrameNumber>& WithinRange, TArray<FFrameNumber>* OutKeyTimes, TArray<FKeyHandle>* OutKeyHandles)
{
    GetData().GetKeys(WithinRange, OutKeyTimes, OutKeyHandles);
}

void FArianeDrawingMovieSceneChannel::GetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<FFrameNumber> OutKeyTimes)
{
    GetData().GetKeyTimes(InHandles, OutKeyTimes);
}

void FArianeDrawingMovieSceneChannel::SetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<const FFrameNumber> InKeyTimes)
{
    GetData().SetKeyTimes(InHandles, InKeyTimes);
}

void FArianeDrawingMovieSceneChannel::DuplicateKeys(TArrayView<const FKeyHandle> InHandles, TArrayView<FKeyHandle> OutNewHandles)
{
    GetData().DuplicateKeys(InHandles, OutNewHandles);
}

void FArianeDrawingMovieSceneChannel::DeleteKeys(TArrayView<const FKeyHandle> InHandles)
{
    GetData().DeleteKeys(InHandles);
}

void FArianeDrawingMovieSceneChannel::DeleteKeysFrom(FFrameNumber InTime, bool bDeleteKeysBefore)
{
    GetData().DeleteKeysFrom(InTime, bDeleteKeysBefore);
}

void FArianeDrawingMovieSceneChannel::ChangeFrameResolution(FFrameRate SourceRate, FFrameRate DestinationRate)
{
    GetData().ChangeFrameResolution(SourceRate, DestinationRate);
}

TRange<FFrameNumber> FArianeDrawingMovieSceneChannel::ComputeEffectiveRange() const
{
    return GetData().GetTotalRange();
}

int32 FArianeDrawingMovieSceneChannel::GetNumKeys() const
{
    return Times.Num();
}

void FArianeDrawingMovieSceneChannel::Reset()
{
    Times.Reset();
    Values.Reset();
    KeyHandles.Reset();
}

void FArianeDrawingMovieSceneChannel::Offset(FFrameNumber DeltaPosition)
{
    GetData().Offset(DeltaPosition);
}

void
FArianeDrawingMovieSceneChannel::PostLoad()
{
    for( FArianeDrawingKeyData& Value : Values )
    {
        Value.PostLoad();
    }
}

void
FArianeDrawingMovieSceneChannel::PostEditUndo()
{
    for( FArianeDrawingKeyData& Value : Values )
    {
        Value.PostEditUndo();
    }
}
