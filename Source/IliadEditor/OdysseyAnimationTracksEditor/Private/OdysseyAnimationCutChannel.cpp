// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCutChannel.h"

#include "Curves/StringCurve.h"
#include "MovieSceneFwd.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "MovieSceneFrameMigration.h"

#include "OdysseyAnimationTimelineSection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationCutChannel)

FOdysseyAnimationCutChannel::FOdysseyAnimationCutChannel()
{
}

FOdysseyAnimationCutChannel::FOdysseyAnimationCutChannel( UOdysseyAnimationTimelineSection* iTimelineSection )
    : TimelineSection( iTimelineSection )
{
}

void
FOdysseyAnimationCutChannel::SetParentSection( UOdysseyAnimationTimelineSection* iTimelineSection )
{
    TimelineSection = iTimelineSection;
}

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

void FOdysseyAnimationCutChannel::SetKeyTimes(TArrayView<const FKeyHandle> iKeyHandles, TArrayView<const FFrameNumber> iNewFrames)
{
#if 1
    struct FKeyHandleAndFrame
    {
        FKeyHandle KeyHandle;
        FFrameNumber OldFrameInSequence;
        FFrameNumber NewFrameInSequence;
        FFrameNumber OldFrameInTimeline;
        FFrameNumber NewFrameInTimeline;
    };

    // Get all data of the wanted keys
    // - the current frames in sequence
    // - the new frames in sequence
    // (and convert to timeline space)
    // - the current frames in timeline
    // - the new frames in timeline
    TArray<FKeyHandleAndFrame> key_and_frames;
    for( int i = 0; i < iKeyHandles.Num(); i++ )
    {
        FKeyHandleAndFrame key_and_frame;
        key_and_frame.KeyHandle = iKeyHandles[i];
        key_and_frame.NewFrameInSequence = iNewFrames[i];
        GetKeyTime( iKeyHandles[i], key_and_frame.OldFrameInSequence );

        key_and_frame.NewFrameInTimeline = TimelineSection->ConvertFrameFromSequenceToTimeline( key_and_frame.NewFrameInSequence );
        key_and_frame.OldFrameInTimeline = TimelineSection->ConvertFrameFromSequenceToTimeline( key_and_frame.OldFrameInSequence );

        key_and_frames.Add( key_and_frame );

        //-

        // This part is only to check to integrity of the frame conversion
        {
            int32 index = GetIndex( key_and_frame.KeyHandle );
            if( !ensure( Values.IsValidIndex( index ) ) )
                continue;

            const FOdysseyAnimationCutValue& value = Values[index];
            const FAnimationCut& animationcut = value.Value;

            FFrameNumber reference_frame = animationcut.GetFrameReference();
            check( reference_frame == key_and_frame.OldFrameInTimeline );
        }
    }

    //---

    // Sort the keys through their current frame
    key_and_frames.Sort( []( const FKeyHandleAndFrame& iElement1, const FKeyHandleAndFrame& iElement2 )
                         {
                             return iElement1.OldFrameInSequence < iElement2.OldFrameInSequence;
                         } );

    // Check the direction of the new frames
    // - direction > 0 : move to right
    // - direction < 0 : move to left
    TSet<int32> offsets;
    int32 direction = 0;
    for( const FKeyHandleAndFrame& key_and_frame : key_and_frames )
    {
        FFrameNumber offset = key_and_frame.NewFrameInSequence - key_and_frame.OldFrameInSequence;
        offsets.Add( offset.Value );

        if( offset > 0 )
            direction = 1;
        else if( offset < 0 )
            direction = -1;
    }
    check( offsets.Num() == 1 );

    // Reverse the order in case they are moved to right
    if( direction > 0 )
        Algo::Reverse( key_and_frames );

    //-

    // Find the real offset to apply to keys
    // The sort of the list is important here:
    // - for a drag to left, the exposure modification must be done from most left cell to most right cell (otherwise, the last cell may not be able to full offset to left as its previous cell will be closer than the offset)
    // - for a drag to right, the exposure modification must be done from most right cell to most left cell (otherwise, the first cell may not be able to full offset to right as its next cell will be closer than the offset)
    TArray<int32> adjusted_offsets_in_timeline;
    for( int i = 0; i < key_and_frames.Num(); i++ )
    {
        const FKeyHandle& keyhandle = key_and_frames[i].KeyHandle;

        int32 index = GetIndex( keyhandle );
        if( !ensure( Values.IsValidIndex( index ) ) )
            continue;
        if( !ensure( Times.IsValidIndex( index ) ) )
            continue;

        //const FFrameNumber& frame = Times[index];
        const FOdysseyAnimationCutValue& value = Values[index];
        const FAnimationCut& animationcut = value.Value;

        int32 desired_offset_in_timeline = ( key_and_frames[i].NewFrameInTimeline - key_and_frames[i].OldFrameInTimeline ).Value;

        int32 adjusted_offset_in_timeline = animationcut.AdjustOffset( desired_offset_in_timeline );

        adjusted_offsets_in_timeline.Add( adjusted_offset_in_timeline );
    }

    // Always get the first offset
    // because it correspond to the first key or the last key (as the list is reordered)
    int32 min_offset_in_timeline = adjusted_offsets_in_timeline[0]; // This won't be ok if keys are not consecutives !

    // Regenerate all new frames by using the valid offset
    for( int i = 0; i < key_and_frames.Num(); i++ )
    {
        key_and_frames[i].NewFrameInTimeline = key_and_frames[i].OldFrameInTimeline + min_offset_in_timeline;
        key_and_frames[i].NewFrameInSequence = TimelineSection->ConvertFrameFromTimelineToSequence( key_and_frames[i].NewFrameInTimeline );
    }

    //---

    // Just rearrange data to set all new frames
    TArray<FKeyHandle> key_handles;
    TArray<FFrameNumber> new_frames_in_sequence;
    for( const FKeyHandleAndFrame& key_and_frame : key_and_frames )
    {
        key_handles.Add( key_and_frame.KeyHandle );
        new_frames_in_sequence.Add( key_and_frame.NewFrameInSequence );
    }

    GetData().SetKeyTimes( key_handles, new_frames_in_sequence );

#else

    GetData().SetKeyTimes( iKeyHandles, iNewFrames );

#endif
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

//---

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

void FOdysseyAnimationCutChannel::Update( const TArray<FKeyHandle>& iKeyHandles, EPropertyChangeType::Type iChangeType )
{
    // At this step, Times have been update with their new frames
    // But Values (animation cuts), aka Cells, are not updated
    // So to really move the Values, just compute the offset between the new frames et the current (old) reference frame of each animation cut

    // Compute the offset
    // At this step, It MUST be the same of all keys
    // SetKeyTimes() should have been called before, and the new frames MUST be correct (clamped, ...)
    TArray<int32> desired_offsets_in_timeline;
    for( int i = 0; i < iKeyHandles.Num(); i++ )
    {
        const FKeyHandle& keyhandle = iKeyHandles[i];

        int32 index = GetIndex( keyhandle );
        if( !ensure( Values.IsValidIndex( index ) ) )
            continue;

        FFrameNumber new_frame_in_sequence = Times[index];
        FFrameNumber new_frame_in_timeline = TimelineSection->ConvertFrameFromSequenceToTimeline( new_frame_in_sequence );

        FOdysseyAnimationCutValue value = Values[index];
        FAnimationCut animationcut = value.Value;

        FFrameNumber old_frame_in_timeline = animationcut.GetFrameReference();

        int32 desired_offset_in_timeline = ( new_frame_in_timeline - old_frame_in_timeline ).Value;
        desired_offsets_in_timeline.Add( desired_offset_in_timeline );
    }

    check( TSet<int32>( desired_offsets_in_timeline ).Num() == 1 );
    int32 desired_offset_in_timeline = desired_offsets_in_timeline[0];

    // Get the direction of the move
    int32 direction = 0;
    if( desired_offset_in_timeline > 0 )
        direction = 1;
    else if( desired_offset_in_timeline < 0 )
        direction = -1;

    //---

    // Build another list to be able to sort it depending of the drag direction
    struct FEntry
    {
        FKeyHandle KeyHandle;
        FAnimationCut AnimationCut;
    };
    TArray<FEntry> sorted_animationcuts;
    for( int i = 0; i < iKeyHandles.Num(); i++ )
    {
        const FKeyHandle& keyhandle = iKeyHandles[i];

        int32 index = GetIndex( keyhandle );
        if( !ensure( Values.IsValidIndex( index ) ) )
            continue;

        FOdysseyAnimationCutValue value = Values[index];
        FAnimationCut animationcut = value.Value;

        FEntry entry = { keyhandle, animationcut };
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
        if( !ensure( Times.IsValidIndex( index ) ) )
            continue;

        FFrameNumber& frame = Times[index];
        FOdysseyAnimationCutValue& value = Values[index];
        FAnimationCut& animationcut = value.Value;

        animationcut.Offset( desired_offset_in_timeline, iChangeType );
    }
}
//
//void FOdysseyAnimationCutChannel::MoveTo( const TArray<FKeyHandle>& iKeyHandles, const TArray<FFrameNumber>& iNewFramesInTimeline, EPropertyChangeType::Type iChangeType )
//{
//    // Generate all the offsets
//    // Offset can be different for each key
//    // When a metakey try to go beyond another metakey limit, the new frame of each subkey will be the same (aka the another metakey limit)
//    // So each subkey offset will be different of each other
//    TArray<int32> desired_offsets_in_timeline;
//    for( int i = 0; i < iKeyHandles.Num(); i++ )
//    {
//        const FKeyHandle& keyhandle = iKeyHandles[i];
//        FFrameNumber new_frame_in_timeline = iNewFramesInTimeline[i];
//
//        int32 index = GetIndex( keyhandle );
//        if( !ensure( Values.IsValidIndex( index ) ) )
//            continue;
//
//        FOdysseyAnimationCutValue value = Values[index];
//        FAnimationCut animationcut = value.Value;
//
//        FFrameNumber reference_frame = animationcut.GetFrameReference();
//
//        int32 desired_offset_in_timeline = ( new_frame_in_timeline - reference_frame ).Value;
//        desired_offsets_in_timeline.Add( desired_offset_in_timeline );
//    }
//
//    check( TSet<int32>( desired_offsets_in_timeline ).Num() <= 1 );
//    int32 desired_offset_in_timeline = desired_offsets_in_timeline.Num() ? desired_offsets_in_timeline[0] : 0;
//
//    //// Find the direction of the drag
//    //// If all offsets are positive -> drag to right
//    //// If all offsets are negative -> drag to left
//    //// Other possibilities should not be possible
//    //bool has_positive_offset = desired_offsets.ContainsByPredicate( []( int32 iEntry )
//    //                                                                {
//    //                                                                    return iEntry > 0;
//    //                                                                } );
//    //bool has_negative_offset = desired_offsets.ContainsByPredicate( []( int32 iEntry )
//    //                                                                {
//    //                                                                    return iEntry < 0;
//    //                                                                } );
//    //// Find the direction:
//    //// 1 = drag to right
//    //// -1 = drag to left
//    //int direction = 0;
//    //if( has_positive_offset && has_negative_offset )
//    //{
//    //    checkNoEntry();
//    //}
//    //else if( has_positive_offset )
//    //{
//    //    direction = 1;
//    //}
//    //else if( has_negative_offset )
//    //{
//    //    direction = -1;
//    //}
//    //else
//    //{
//    //    //return;
//    //    direction = 1;
//    //}
//
//    int direction = 0;
//    if( desired_offset_in_timeline > 0 )
//        direction = 1;
//    else if( desired_offset_in_timeline < 0 )
//        direction = -1;
//
//    //---
//    //
//    // Build another list to be able to sort it depending of the drag direction
//    struct FEntry
//    {
//        FKeyHandle KeyHandle;
//        FAnimationCut AnimationCut;
//    };
//    TArray<FEntry> sorted_animationcuts;
//    for( int i = 0; i < iKeyHandles.Num(); i++ )
//    {
//        const FKeyHandle& keyhandle = iKeyHandles[i];
//
//        int32 index = GetIndex( keyhandle );
//        if( !ensure( Values.IsValidIndex( index ) ) )
//            continue;
//
//        FOdysseyAnimationCutValue value = Values[index];
//        FAnimationCut animationcut = value.Value;
//
//        FEntry entry = { keyhandle, animationcut };
//        sorted_animationcuts.Add( entry );
//    }
//
//    // Always sort it by each frame reference
//    sorted_animationcuts.Sort( [this]( const FEntry& iEntry1, const FEntry& iEntry2 )
//                               {
//                                   return iEntry1.AnimationCut.GetFrameReference() < iEntry2.AnimationCut.GetFrameReference();
//                               } );
//
//    // Reverse the order when a drag to right occurs
//    if( direction > 0 )
//    {
//        Algo::Reverse( sorted_animationcuts );
//    }
//
//    // Modify all cell exposure of dragged subkeys
//    // The sort of the list is important here:
//    // - for a drag to left, the exposure modification must be done from most left cell to most right cell (otherwise, the last cell may not be able to full offset to left as its previous cell will be closer than the offset)
//    // - for a drag to right, the exposure modification must be done from most right cell to most left cell (otherwise, the first cell may not be able to full offset to right as its next cell will be closer than the offset)
//    for( int i = 0; i < sorted_animationcuts.Num(); i++ )
//    {
//        const FKeyHandle& keyhandle = sorted_animationcuts[i].KeyHandle;
//
//        int32 index = GetIndex( keyhandle );
//        if( !ensure( Values.IsValidIndex( index ) ) )
//            continue;
//        if( !ensure( Times.IsValidIndex( index ) ) )
//            continue;
//
//        FFrameNumber& frame = Times[index];
//        FOdysseyAnimationCutValue& value = Values[index];
//        FAnimationCut& animationcut = value.Value;
//
//        animationcut.Offset( desired_offset_in_timeline, iChangeType );
//    }
//
//    //// Modify all cell exposure of dragged subkeys
//    //// The sort of the list is important here:
//    //// - for a drag to left, the exposure modification must be done from most left cell to most right cell (otherwise, the last cell may not be able to full offset to left as its previous cell will be closer than the offset)
//    //// - for a drag to right, the exposure modification must be done from most right cell to most left cell (otherwise, the first cell may not be able to full offset to right as its next cell will be closer than the offset)
//    //for( int i = 0; i < sorted_animationcuts.Num(); i++ )
//    //{
//    //    const FKeyHandle& keyhandle = sorted_animationcuts[i].KeyHandle;
//
//    //    int32 index = GetIndex( keyhandle );
//    //    if( !ensure( Values.IsValidIndex( index ) ) )
//    //        continue;
//
//    //    FOdysseyAnimationCutValue& value = Values[index];
//    //    FAnimationCut& animationcut = value.Value;
//
//    //    animationcut.Offset( iOffset.Value, iChangeType );
//    //}
//}
