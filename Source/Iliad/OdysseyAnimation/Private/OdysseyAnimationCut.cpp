// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCut.h"

#include "MovieSceneTimeHelpers.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationUtils.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerCell.h"

#define LOCTEXT_NAMESPACE "AnimationCut"

//---

FAnimationCutEntry::FAnimationCutEntry()
{
}

FAnimationCutEntry::FAnimationCutEntry( UOdysseyLayerCell* iCellBefore, UOdysseyLayerCell* iCellAfter )
    : mCellBefore( iCellBefore )
    , mCellAfter( iCellAfter )
{
}

FFrameNumber
FAnimationCutEntry::GetFrameReference() const
{
    if( !mCellBefore && mCellAfter )
        return mCellAfter->GetFrameRange().GetLowerBoundValue(); // Always inclusive

    if( mCellBefore && mCellAfter )
        return mCellAfter->GetFrameRange().GetLowerBoundValue(); // Always inclusive

    if( mCellBefore && !mCellAfter )
        return UE::MovieScene::DiscreteExclusiveUpper( TRange<FFrameNumber>::Inclusive( mCellBefore->GetFrameRange().GetLowerBoundValue(), mCellBefore->GetFrameRange().GetUpperBoundValue() ) ); // GetFrameRange(): always inclusive

    checkNoEntry();
    return FFrameNumber();
}

UOdysseyLayerCell*
FAnimationCutEntry::GetCellReference()
{
    if( !mCellBefore && mCellAfter )
        return mCellAfter;

    if( mCellBefore && mCellAfter )
        return mCellAfter;

    if( mCellBefore && !mCellAfter )
        return mCellBefore;

    checkNoEntry();
    return nullptr;
}

TRange<FFrameNumber>
FAnimationCutEntry::GetRangeLimit() const
{
    TRange<FFrameNumber> range = TRange<FFrameNumber>::AtLeast( 0 );
    if( mCellBefore )
        range.SetLowerBound( TRangeBound<FFrameNumber>::Inclusive( mCellBefore->GetFrameRange().GetLowerBoundValue() + 1 ) ); // Always inclusive
    if( mCellAfter )
        range.SetUpperBound( TRangeBound<FFrameNumber>::Inclusive( mCellAfter->GetFrameRange().GetUpperBoundValue() ) ); // Always inclusive

    return range;
}

void
FAnimationCutEntry::SetFrame( FFrameNumber iNewFrame, EPropertyChangeType::Type iChangeType )
{
    FFrameNumber reference_frame = GetFrameReference();
    int32 offset = FMath::Abs( reference_frame - iNewFrame ).Value;

    if( iNewFrame < reference_frame )
    {
        if( !mCellBefore && mCellAfter )
        {
#if WITH_EDITOR
            if( iChangeType == EPropertyChangeType::Interactive )
            {
                mCellAfter->SetExposureInteractive( mCellAfter->GetExposure() + offset );
                mCellAfter->GetLayer()->SetCellsOffsetInteractive( mCellAfter->GetLayer()->GetCellsOffset() - offset );
            }
            else
#endif
            {
                mCellAfter->SetExposure( mCellAfter->GetExposure() + offset );
                mCellAfter->GetLayer()->SetCellsOffset( mCellAfter->GetLayer()->GetCellsOffset() - offset );
            }
        }

        if( mCellBefore && mCellAfter )
        {
#if WITH_EDITOR
            if( iChangeType == EPropertyChangeType::Interactive )
            {
                mCellBefore->SetExposureInteractive( mCellBefore->GetExposure() - offset );
                mCellAfter->SetExposureInteractive( mCellAfter->GetExposure() + offset );
            }
            else
#endif
            {
                mCellBefore->SetExposure( mCellBefore->GetExposure() - offset );
                mCellAfter->SetExposure( mCellAfter->GetExposure() + offset );
            }
        }

        if( mCellBefore && !mCellAfter )
        {
#if WITH_EDITOR
            if( iChangeType == EPropertyChangeType::Interactive )
            {
                mCellBefore->SetExposureInteractive( mCellBefore->GetExposure() - offset );
            }
            else
#endif
            {
                mCellBefore->SetExposure( mCellBefore->GetExposure() - offset );
            }
        }
    }
    else if( iNewFrame > reference_frame )
    {
        if( !mCellBefore && mCellAfter )
        {
#if WITH_EDITOR
            if( iChangeType == EPropertyChangeType::Interactive )
            {
                mCellAfter->SetExposureInteractive( mCellAfter->GetExposure() - offset );
                mCellAfter->GetLayer()->SetCellsOffsetInteractive( mCellAfter->GetLayer()->GetCellsOffset() + offset );
            }
            else
#endif
            {
                mCellAfter->SetExposure( mCellAfter->GetExposure() - offset );
                mCellAfter->GetLayer()->SetCellsOffset( mCellAfter->GetLayer()->GetCellsOffset() + offset );
            }
        }

        if( mCellBefore && mCellAfter )
        {
#if WITH_EDITOR
            if( iChangeType == EPropertyChangeType::Interactive )
            {
                mCellBefore->SetExposureInteractive( mCellBefore->GetExposure() + offset );
                mCellAfter->SetExposureInteractive( mCellAfter->GetExposure() - offset );
            }
            else
#endif
            {
                mCellBefore->SetExposure( mCellBefore->GetExposure() + offset );
                mCellAfter->SetExposure( mCellAfter->GetExposure() - offset );
            }
        }

        if( mCellBefore && !mCellAfter )
        {
#if WITH_EDITOR
            if( iChangeType == EPropertyChangeType::Interactive )
            {
                mCellBefore->SetExposureInteractive( mCellBefore->GetExposure() + offset );
            }
            else
#endif
            {
                mCellBefore->SetExposure( mCellBefore->GetExposure() + offset );
            }
        }
    }
    else
    {
        if( mCellAfter )
        {
#if WITH_EDITOR
            if( iChangeType == EPropertyChangeType::Interactive )
            {
                mCellAfter->SetExposureInteractive( mCellAfter->GetExposure() );
            }
            else
#endif
            {
                mCellAfter->SetExposure( mCellAfter->GetExposure() );
            }
        }

        if( mCellBefore )
        {
#if WITH_EDITOR
            if( iChangeType == EPropertyChangeType::Interactive )
            {
                mCellBefore->SetExposureInteractive( mCellBefore->GetExposure() );
            }
            else
#endif
            {
                mCellBefore->SetExposure( mCellBefore->GetExposure() );
            }
        }
    }
}

//---

FAnimationCut::FAnimationCut()
{
}

UOdysseyAnimation*
FAnimationCut::GetAnimation()
{
    for( FAnimationCutEntry entry : mAnimationCutEntries )
    {
        if( !entry.GetCellReference() )
            continue;

        UOdysseyAnimation* animation = ::Odyssey::AnimationUtils::GetCellAnimation(entry.GetCellReference());
        if (animation)
            return animation;
    }

    return nullptr;
}

FFrameNumber
FAnimationCut::GetFrameReference() const
{
    TSet<FFrameNumber> frames;
    for( FAnimationCutEntry entry : mAnimationCutEntries )
        frames.Add( entry.GetFrameReference() );
    check( frames.Num() == 1 );

    return frames.Array()[0];
}

TArray<UOdysseyLayerCell*>
FAnimationCut::GetCellsReference() const
{
    TArray<UOdysseyLayerCell*> cells;
    for( FAnimationCutEntry entry : mAnimationCutEntries )
        cells.Add( entry.GetCellReference() );

    return cells;
}

TRange<FFrameNumber>
FAnimationCut::GetRangeLimit() const
{
    if( mAnimationCutEntries.IsEmpty() )
        return TRange<FFrameNumber>::All();

    TArray<TRange<FFrameNumber>> ranges;
    for( FAnimationCutEntry entry : mAnimationCutEntries )
    {
        TRange<FFrameNumber> range_limit = entry.GetRangeLimit();
        ranges.Add( range_limit );
    }

    TRange<FFrameNumber> range = TRange<FFrameNumber>::Hull( ranges );
    //TRange<FFrameNumber> range = TRange<FFrameNumber>::Intersection( ranges );
    return range;
}

//void
//FAnimationCut::Update( FFrameNumber iNewFrame, EPropertyChangeType::Type iChangeType )
//{
//    TRange<FFrameNumber> range_limit = GetRangeLimit();
//    FFrameNumber frame_reference = GetFrameReference();
//    FFrameNumber new_frame = frame_reference + iOffset;
//
//    int32 clamped_offset = iOffset;
//    if( iOffset < 0 )
//    {
//        if( range_limit.GetLowerBound().IsClosed() )
//        {
//            FFrameNumber lower_frame_limit = range_limit.GetLowerBoundValue(); // Always inclusive
//            if( new_frame < lower_frame_limit )
//            {
//                clamped_offset = ( lower_frame_limit - frame_reference ).Value;
//                new_frame = frame_reference + clamped_offset;
//            }
//        }
//    }
//    else if( iOffset > 0 )
//    {
//        if( range_limit.GetUpperBound().IsClosed() )
//        {
//            FFrameNumber upper_frame_limit = range_limit.GetUpperBoundValue(); // Always inclusive
//            if( new_frame > upper_frame_limit )
//            {
//                clamped_offset = ( upper_frame_limit - frame_reference ).Value;
//                new_frame = frame_reference + clamped_offset;
//            }
//        }
//    }
//
//    for( FAnimationCutEntry& entry : mAnimationCutEntries )
//    {
//        entry.Update( new_frame, iChangeType );
//    }
//}

void
FAnimationCut::Offset( int32 iOffset, EPropertyChangeType::Type iChangeType )
{
    int32 adjusted_offset = AdjustOffset( iOffset );

    FFrameNumber new_frame = GetFrameReference() + adjusted_offset;

    for( FAnimationCutEntry& entry : mAnimationCutEntries )
    {
        entry.SetFrame( new_frame, iChangeType );
    }
}

int32
FAnimationCut::AdjustOffset( int32 iOffset ) const
{
    TRange<FFrameNumber> range_limit = GetRangeLimit();
    FFrameNumber frame_reference = GetFrameReference();
    FFrameNumber new_frame = frame_reference + iOffset;

    int32 clamped_offset = iOffset;
    if( iOffset < 0 )
    {
        if( range_limit.GetLowerBound().IsClosed() )
        {
            FFrameNumber lower_frame_limit = range_limit.GetLowerBoundValue(); // Always inclusive
            if( new_frame < lower_frame_limit )
            {
                clamped_offset = ( lower_frame_limit - frame_reference ).Value;
                new_frame = frame_reference + clamped_offset;
            }
        }
    }
    else if( iOffset > 0 )
    {
        if( range_limit.GetUpperBound().IsClosed() )
        {
            FFrameNumber upper_frame_limit = range_limit.GetUpperBoundValue(); // Always inclusive
            if( new_frame > upper_frame_limit )
            {
                clamped_offset = ( upper_frame_limit - frame_reference ).Value;
                new_frame = frame_reference + clamped_offset;
            }
        }
    }

    return clamped_offset;
}

void
FAnimationCut::AddNewEntry( const FAnimationCutEntry& iAnimationCutEntry )
{
    mAnimationCutEntries.Add( iAnimationCutEntry );
}

#undef LOCTEXT_NAMESPACE
