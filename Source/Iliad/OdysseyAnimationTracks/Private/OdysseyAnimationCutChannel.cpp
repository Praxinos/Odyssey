// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#include "OdysseyAnimationCutChannel.h"

#include "Curves/StringCurve.h"
#include "MovieSceneFwd.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "MovieSceneFrameMigration.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationCutChannel)

const FOdysseyAnimationCutValue* FOdysseyAnimationCutChannel::Evaluate(FFrameTime InTime) const
{
    if (Times.Num())
    {
        const int32 Index = FMath::Max(0, Algo::UpperBound(Times, InTime.FrameNumber)-1);
        return &Values[Index];
    }

    return nullptr;
}

bool FOdysseyAnimationCutChannel::Evaluate( FFrameTime InTime, FOdysseyAnimationCutValue& OutValue ) const
{
    if( Times.Num() )
    {
        const int32 Index = FMath::Max( 0, Algo::UpperBound( Times, InTime.FrameNumber ) - 1 );
        OutValue = Values[Index];
        return true;
    }

    return false;
}

void FOdysseyAnimationCutChannel::GetKeys(const TRange<FFrameNumber>& WithinRange, TArray<FFrameNumber>* OutKeyTimes, TArray<FKeyHandle>* OutKeyHandles)
{
    GetData().GetKeys(WithinRange, OutKeyTimes, OutKeyHandles);
}

void FOdysseyAnimationCutChannel::GetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<FFrameNumber> OutKeyTimes)
{
    GetData().GetKeyTimes(InHandles, OutKeyTimes);
}

void FOdysseyAnimationCutChannel::SetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<const FFrameNumber> InKeyTimes)
{
    GetData().SetKeyTimes(InHandles, InKeyTimes);
}

void FOdysseyAnimationCutChannel::DuplicateKeys(TArrayView<const FKeyHandle> InHandles, TArrayView<FKeyHandle> OutNewHandles)
{
    GetData().DuplicateKeys(InHandles, OutNewHandles);
}

void FOdysseyAnimationCutChannel::DeleteKeys(TArrayView<const FKeyHandle> InHandles)
{
    GetData().DeleteKeys(InHandles);
}

void FOdysseyAnimationCutChannel::DeleteKeysFrom(FFrameNumber InTime, bool bDeleteKeysBefore)
{
    // Insert a key at the current time to maintain evaluation
    if (GetData().GetTimes().Num() > 0)
    {
        FOdysseyAnimationCutValue Value;
        if( Evaluate( InTime, Value ) )
        {
            GetData().UpdateOrAddKey( InTime, Value );
        }
    }

    GetData().DeleteKeysFrom(InTime, bDeleteKeysBefore);
}

FKeyHandle FOdysseyAnimationCutChannel::GetHandle(int32 Index)
{
    return GetData().GetHandle(Index);
}

int32 FOdysseyAnimationCutChannel::GetIndex(FKeyHandle Handle)
{
    return GetData().GetIndex(Handle);
}

void FOdysseyAnimationCutChannel::RemapTimes(const UE::MovieScene::IRetimingInterface& Retimer)
{
    GetData().RemapTimes(Retimer);
}

TRange<FFrameNumber> FOdysseyAnimationCutChannel::ComputeEffectiveRange() const
{
    return GetData().GetTotalRange();
}

int32 FOdysseyAnimationCutChannel::GetNumKeys() const
{
    return Times.Num();
}

void FOdysseyAnimationCutChannel::Reset()
{
    Times.Reset();
    Values.Reset();
    KeyHandles.Reset();
}

//void FOdysseyAnimationCutChannel::Optimize(const FKeyDataOptimizationParams& InParameters)
//{
//  UE::MovieScene::Optimize(this, InParameters);
//}

void FOdysseyAnimationCutChannel::Offset(FFrameNumber DeltaPosition)
{
    GetData().Offset(DeltaPosition);
}
