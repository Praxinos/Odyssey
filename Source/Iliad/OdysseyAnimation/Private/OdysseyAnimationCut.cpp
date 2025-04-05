// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#include "OdysseyAnimationCut.h"

#include "MovieSceneTimeHelpers.h"

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimation.h"

#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "AnimationCut"

//---

FAnimationCutEntry::FAnimationCutEntry()
{
}

FAnimationCutEntry::FAnimationCutEntry( UOdysseyAnimationCell* iCellBefore, UOdysseyAnimationCell* iCellAfter )
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

UOdysseyAnimationCell*
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
FAnimationCutEntry::SetFrame( FFrameNumber iNewFrame )
{
    FFrameNumber reference_frame = GetFrameReference();
    int32 offset = FMath::Abs( reference_frame - iNewFrame ).Value;

    if( iNewFrame < reference_frame )
    {
        if( !mCellBefore && mCellAfter )
        {
            FOdysseyObjectEditorUtils::SetPropertyValue( mCellAfter, GET_MEMBER_NAME_CHECKED( UOdysseyAnimationCell, Exposure ), mCellAfter->Exposure + offset );

            FOdysseyObjectEditorUtils::SetPropertyValue( mCellAfter->GetLayer(), GET_MEMBER_NAME_CHECKED( UOdysseyAnimationLayer, CellsOffset ), mCellAfter->GetLayer()->CellsOffset - offset );
        }

        if( mCellBefore && mCellAfter )
        {
            FOdysseyObjectEditorUtils::SetPropertyValue( mCellBefore, GET_MEMBER_NAME_CHECKED( UOdysseyAnimationCell, Exposure ), mCellBefore->Exposure - offset );

            FOdysseyObjectEditorUtils::SetPropertyValue( mCellAfter, GET_MEMBER_NAME_CHECKED( UOdysseyAnimationCell, Exposure ), mCellAfter->Exposure + offset );
        }

        if( mCellBefore && !mCellAfter )
        {
            FOdysseyObjectEditorUtils::SetPropertyValue( mCellBefore, GET_MEMBER_NAME_CHECKED( UOdysseyAnimationCell, Exposure ), mCellBefore->Exposure - offset );
        }
    }
    else
    {
        if( !mCellBefore && mCellAfter )
        {
            FOdysseyObjectEditorUtils::SetPropertyValue( mCellAfter, GET_MEMBER_NAME_CHECKED( UOdysseyAnimationCell, Exposure ), mCellAfter->Exposure - offset );

            FOdysseyObjectEditorUtils::SetPropertyValue( mCellAfter->GetLayer(), GET_MEMBER_NAME_CHECKED( UOdysseyAnimationLayer, CellsOffset ), mCellAfter->GetLayer()->CellsOffset + offset );
        }

        if( mCellBefore && mCellAfter )
        {
            FOdysseyObjectEditorUtils::SetPropertyValue( mCellBefore, GET_MEMBER_NAME_CHECKED( UOdysseyAnimationCell, Exposure ), mCellBefore->Exposure + offset );

            FOdysseyObjectEditorUtils::SetPropertyValue( mCellAfter, GET_MEMBER_NAME_CHECKED( UOdysseyAnimationCell, Exposure ), mCellAfter->Exposure - offset );
        }

        if( mCellBefore && !mCellAfter )
        {
            FOdysseyObjectEditorUtils::SetPropertyValue( mCellBefore, GET_MEMBER_NAME_CHECKED( UOdysseyAnimationCell, Exposure ), mCellBefore->Exposure + offset );
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
        if( entry.GetCellReference() && entry.GetCellReference()->GetAnimation() )
            return entry.GetCellReference()->GetAnimation();
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

TArray<UOdysseyAnimationCell*>
FAnimationCut::GetCellsReference() const
{
    TArray<UOdysseyAnimationCell*> cells;
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

int32
FAnimationCut::Offset( int32 iOffset )
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

    for( FAnimationCutEntry entry : mAnimationCutEntries )
    {
        entry.SetFrame( new_frame );
    }

    return clamped_offset;
}

void
FAnimationCut::AddNewEntry( const FAnimationCutEntry& iAnimationCutEntry )
{
    mAnimationCutEntries.Add( iAnimationCutEntry );
}

//---

FAnimationCuts::FAnimationCuts()
{
}

FAnimationCuts::FAnimationCuts( UOdysseyAnimation* iAnimation )
    : mAnimation( iAnimation )
{
}

void
FAnimationCuts::Build()
{
    UOdysseyAnimationLayerStack* layer_stack = mAnimation ? mAnimation->GetLayerStack() : nullptr;
    TArray<UOdysseyLayer*> layers = layer_stack ? layer_stack->GetLayers() : TArray<UOdysseyLayer*>();
    TSet<UOdysseyAnimationLayer*> animation_layers;
    for( UOdysseyLayer* layer : layers )
        animation_layers.Add( Cast<UOdysseyAnimationLayer>( layer ) );
    animation_layers.Remove( nullptr );

    for( UOdysseyAnimationLayer* layer : animation_layers )
    {
        UOdysseyAnimationCell* previous_cell = nullptr;
        TArray<UOdysseyAnimationCell*> cells = layer->GetCells();
        for( UOdysseyAnimationCell* cell : cells )
        {
            {
                FAnimationCutEntry animationcutentry( previous_cell, cell );

                FAnimationCut* animationcut = mAnimationCutPerFrameMap.Find( animationcutentry.GetFrameReference() );
                if( animationcut )
                {
                    animationcut->AddNewEntry( animationcutentry );
                }
                else
                {
                    FAnimationCut new_animationcut;
                    new_animationcut.AddNewEntry( animationcutentry );

                    mAnimationCutPerFrameMap.Add( animationcutentry.GetFrameReference(), new_animationcut );
                }
            }

            previous_cell = cell;

            if( cell == cells.Last() )
            {
                FAnimationCutEntry animationcutentry( previous_cell, nullptr );

                FAnimationCut* animationcut = mAnimationCutPerFrameMap.Find( animationcutentry.GetFrameReference() );
                if( animationcut )
                {
                    animationcut->AddNewEntry( animationcutentry );
                }
                else
                {
                    FAnimationCut new_animationcut;
                    new_animationcut.AddNewEntry( animationcutentry );

                    mAnimationCutPerFrameMap.Add( animationcutentry.GetFrameReference(), new_animationcut );
                }
            }
        }
    }

    mAnimationCutPerFrameMap.KeyStableSort( []( FFrameNumber iA, FFrameNumber iB )
                                    {
                                        return iA < iB;
                                    } );
}

const TMap<FFrameNumber, FAnimationCut>&
FAnimationCuts::GetMap() const
{
    return mAnimationCutPerFrameMap;
}

bool
FAnimationCuts::FindAnimationCutKey( const FAnimationCut& iAnimationCut, FFrameNumber& oFrame ) const
{
    const FFrameNumber* frame = mAnimationCutPerFrameMap.FindKey( iAnimationCut );
    if( !frame )
        return false;

    oFrame = *frame;
    return true;
}

bool
FAnimationCuts::FindPreviousAnimationCut( const FAnimationCut& iAnimationCut, FAnimationCut& oPreviousAnimationCut ) const
{
    bool has_previous_frame = false;

    FFrameNumber frame;
    if( !FindAnimationCutKey( iAnimationCut, frame ) )
        return has_previous_frame;

    TArray<FFrameNumber> keys;
    mAnimationCutPerFrameMap.GetKeys( keys );
    for( FFrameNumber key : keys )
    {
        if( frame > key )
        {
            has_previous_frame = true;
            oPreviousAnimationCut = mAnimationCutPerFrameMap[key];
            continue;
        }

        break;
    }

    return has_previous_frame;
}

bool
FAnimationCuts::FindNextAnimationCut( const FAnimationCut& iAnimationCut, FAnimationCut& oNextAnimationCut ) const
{
    bool has_next_frame = false;

    FFrameNumber frame;
    if( !FindAnimationCutKey( iAnimationCut, frame ) )
        return has_next_frame;

    TArray<FFrameNumber> keys;
    mAnimationCutPerFrameMap.GetKeys( keys );
    Algo::Reverse( keys ); // Reverse to start from end !
    for( FFrameNumber key : keys )
    {
        if( frame < key )
        {
            has_next_frame = true;
            oNextAnimationCut = mAnimationCutPerFrameMap[key];
            continue;
        }

        break;
    }

    return has_next_frame;
}

void
FAnimationCuts::UpdateAnimationCuts( const TArray<FAnimationCut>& iAnimationCuts, const TArray<FFrameNumber>& iNewFrames )
{
    TSet<int32> offsets;
    for( int i = 0; i < iAnimationCuts.Num(); i++ )
    {
        const FAnimationCut& animationcut_moved = iAnimationCuts[i];
        FFrameNumber new_frame = iNewFrames[i];

        FFrameNumber reference_frame = animationcut_moved.GetFrameReference();

        offsets.Add( ( new_frame - reference_frame ).Value );
    }
    check( offsets.Num() == 1 );

    int32 offset = offsets.Array()[0];

    //---

    for( int i = 0; i < iAnimationCuts.Num(); i++ )
    {
        const FAnimationCut& animationcut_moved = iAnimationCuts[i];
        FFrameNumber new_frame = iNewFrames[i];

        FFrameNumber frame;
        bool found = FindAnimationCutKey( animationcut_moved, frame );
        if( !ensure( found ) )
            continue;

        FAnimationCut& animationcut = mAnimationCutPerFrameMap.FindChecked( frame );

        offset = animationcut.Offset( offset );
    }
}

#undef LOCTEXT_NAMESPACE
