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

FKeyHandle FOdysseyAnimationCutChannel::FindPreviousKey( const TArray<FKeyHandle>& iKeyHandles )
{
    TArray<int32> indexes;
    for( FKeyHandle key : iKeyHandles )
        indexes.Add( GetIndex( key ) );

    if( indexes.IsEmpty() )
        return FKeyHandle::Invalid();

    int32 index_min = FMath::Min( indexes );

    int32 index_previous = index_min - 1;
    if( !Times.IsValidIndex( index_previous ) )
        return FKeyHandle::Invalid();

    return GetHandle( index_previous );
}

FKeyHandle FOdysseyAnimationCutChannel::FindNextKey( const TArray<FKeyHandle>& iKeyHandles )
{
    TArray<int32> indexes;
    for( FKeyHandle key : iKeyHandles )
        indexes.Add( GetIndex( key ) );

    if( indexes.IsEmpty() )
        return FKeyHandle::Invalid();

    int32 index_max = FMath::Max( indexes );

    int32 index_next = index_max + 1;
    if( !Times.IsValidIndex( index_next ) )
        return FKeyHandle::Invalid();

    return GetHandle( index_next );
}

void FOdysseyAnimationCutChannel::MoveTo( const TArray<FKeyHandle>& iKeyHandles, const TArray<FFrameNumber>& iNewFrames )
{
    // Generate all the offsets
    // Offset can be different for each key
    // When a metakey try to go beyond another metakey limit, the new frame of each subkey will be the same (aka the another metakey limit)
    // So each subkey offset will be different of each other
    TArray<int32> desired_offsets;
    for( int i = 0; i < iKeyHandles.Num(); i++ )
    {
        const FKeyHandle& keyhandle = iKeyHandles[i];
        FFrameNumber new_frame = iNewFrames[i];

        int32 index = GetIndex( keyhandle );
        if( !ensure( Values.IsValidIndex( index ) ) )
            continue;

        FOdysseyAnimationCutValue value = Values[index];
        FAnimationCut animationcut = value.Value;

        FFrameNumber reference_frame = animationcut.GetFrameReference();

        int32 desired_offset = ( new_frame - reference_frame ).Value;
        desired_offsets.Add( desired_offset );
    }

    // Find the direction of the drag
    // If all offsets are positive -> drag to right
    // If all offsets are negative -> drag to left
    // Other possibilities should not be possible
    bool has_positive_offset = desired_offsets.ContainsByPredicate( []( int32 iEntry )
                                                                    {
                                                                        return iEntry > 0;
                                                                    } );
    bool has_negative_offset = desired_offsets.ContainsByPredicate( []( int32 iEntry )
                                                                    {
                                                                        return iEntry < 0;
                                                                    } );
    // Find the direction:
    // 1 = drag to right
    // -1 = drag to left
    int direction = 0;
    if( has_positive_offset && has_negative_offset )
    {
        checkNoEntry();
    }
    else if( has_positive_offset )
    {
        direction = 1;
    }
    else if( has_negative_offset )
    {
        direction = -1;
    }
    else
    {
        return;
    }

    // Always get the closest offset to 0
    // (At this step, it's sure to only have full positive or full negative offset)
    int32 offset = FMath::Min( desired_offsets );
    if( direction < 0 )
        offset = FMath::Max( desired_offsets );

    //---

    // Build another list to be able to sort it depending of the drag direction
    struct FEntry
    {
        FKeyHandle KeyHandle;
        FAnimationCut AnimationCut;
        FFrameNumber NewFrame;
    };
    TArray<FEntry> sorted_animationcuts;
    for( int i = 0; i < iKeyHandles.Num(); i++ )
    {
        const FKeyHandle& keyhandle = iKeyHandles[i];
        FFrameNumber new_frame = iNewFrames[i];

        int32 index = GetIndex( keyhandle );
        if( !ensure( Values.IsValidIndex( index ) ) )
            continue;

        FOdysseyAnimationCutValue value = Values[index];
        FAnimationCut animationcut = value.Value;

        FEntry entry = { keyhandle, animationcut, new_frame };
        sorted_animationcuts.Add( entry );
    }

    // Always sort it by each frame reference
    sorted_animationcuts.Sort( [this]( const FEntry& iEntry1, const FEntry& iEntry2 )
                               {
                                   return iEntry1.AnimationCut.GetFrameReference() < iEntry2.AnimationCut.GetFrameReference();
                               } );

    // Reverse the order when a drag to right occurs
    if( direction > 0 )
    {
        Algo::Reverse( sorted_animationcuts );
        //TArray<FEntry> tmp;
        //for( FEntry entry : sorted_animationcuts )
        //    tmp.Insert( entry, 0 );
        //sorted_animationcuts = tmp;
    }

    // Modify all cell exposure of dragged subkeys
    // The sort of the list is important here:
    // - for a drag to left, the exposure modification must be done from most left cell to most right cell (otherwise, the last cell may not be able to full offset to left as its previous cell will be closer than the offset)
    // - for a drag to right, the exposure modification must be done from most right cell to most left cell (otherwise, the first cell may not be able to full offset to right as its next cell will be closer than the offset)
    for( int i = 0; i < sorted_animationcuts.Num(); i++ )
    {
        const FKeyHandle& keyhandle = sorted_animationcuts[i].KeyHandle;

        int32 index = GetIndex( keyhandle );
        if( !ensure( Values.IsValidIndex( index ) ) )
            continue;

        FOdysseyAnimationCutValue& value = Values[index];
        FAnimationCut& animationcut = value.Value;

        animationcut.Offset( offset );
    }
}
