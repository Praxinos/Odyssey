// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "CinematicBoardWidgets/SMetaKeysArea.h"

#include "Brushes/SlateColorBrush.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "CineCameraActor.h"
#include "MVVM/Views/KeyDrawParams.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "SequencerSettings.h"
#include "TimeToPixel.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MetaChannelProxy.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "SMetaKeysArea"

//---

void
SMetaKeysArea::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    // This function is only used to set inner variables
    // InArgs should never be used here as this Construct() is always called in child class with SMetaKeysArea::FArguments(), so InArgs is always 'empty'

    mBoardSection = iBoardSection;

    SetToolTipText( MakeAttributeSP( this, &SMetaKeysArea::GetTooltipText ) );
}

FVector2D
SMetaKeysArea::ComputeDesiredSize( float ) const //override
{
    FVector2D size = GetDesiredSize();
    size.Y = SequencerSectionConstants::DefaultSectionHeight + 5.f;

    return size;
}

//---

FText
SMetaKeysArea::GetTooltipText() const
{
    if( mHoveredKeys.IsValid() && mHoveredKeys->NumMetaKeys() )
        return GetKeyTooltipText( mHoveredKeys );

    return GetAreaTooltipText();
}

FText
SMetaKeysArea::GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const
{
    return FText::GetEmpty();
}

FText
SMetaKeysArea::GetAreaTooltipText() const
{
    return FText::GetEmpty();
}

SMetaKeysArea::EDragMode
SMetaKeysArea::InitDragMode() const
{
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
        return EDragMode::kShiftFromKey;

    return EDragMode::kMoveSingleKey;
}

TSharedPtr<FMetaChannel>
SMetaKeysArea::CreateKeysUnderMouse( const FPointerEvent& MouseEvent ) const
{
    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();

    const FMovieSceneSequenceTransform OuterToInnerTransform = subsection_object->OuterToInnerTransform();

    FGeometry geometry;
    FTimeToPixel converter = board_section->ConstructConverterForViewRange( &geometry );
    FFrameTime clicked_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    //---

    FFrameTime inner_clicked_frame = clicked_frame * OuterToInnerTransform;

    //---

    mDragMode = InitDragMode();

    TSharedPtr<const FMetaChannel> meta_channel = GetMetaChannel();
    if( !meta_channel )
        return nullptr;

    if( mDragMode == EDragMode::kShiftFromKey )
        return meta_channel->CreateFromTimeAndBeyond( inner_clicked_frame, inner_tolerance );

    return meta_channel->CreateFromTime( inner_clicked_frame, inner_tolerance );
}

void
SMetaKeysArea::BeginTransaction( const FText& iTransactionDesc ) // From FEditToolDragOperation::BeginTransaction#95
{
    // Begin an editor transaction and mark the section as transactional so it's state will be saved
    mTransaction.Reset( new FScopedTransaction( iTransactionDesc ) );

    //---

    for( auto pair : mDraggedKeys->GetMetaKeys() )
    {
        for( const auto& subkey : pair.Value.mSubKeys )
        {
            UMovieSceneSection* section = subkey.mSection.Get();
            if( !section )
                continue;

            section->SetFlags( RF_Transactional );
            // Save the current state of the section
            section->TryModify();
        }
    }
}

void
SMetaKeysArea::EndTransaction()
{
    mTransaction.Reset();

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    ISequencer*             sequencer = board_section->GetSequencer().Get();

    sequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

bool
SMetaKeysArea::BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys )
{
    return false;
}

//---

void
SMetaKeysArea::OnStartDragKeys( TSharedPtr<FMetaChannel> iKeys )
{
}

void
SMetaKeysArea::OnDragKeys( TSharedPtr<FMetaChannel> iKeys )
{
}

void
SMetaKeysArea::OnStopDragKeys( TSharedPtr<FMetaChannel> iKeys )
{
}

void
SMetaKeysArea::OnClickKeys( TSharedPtr<FMetaChannel> iKeys )
{
}

//---

FCursorReply
SMetaKeysArea::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    TSharedPtr<FMetaChannel> meta_channel = CreateKeysUnderMouse( CursorEvent );

    if( meta_channel.IsValid() && meta_channel->NumMetaKeys() )
    {
        if( mDragMode == EDragMode::kShiftFromKey )
        {
            return FCursorReply::Cursor( EMouseCursor::GrabHand );
        }

        return FCursorReply::Cursor( EMouseCursor::ResizeLeftRight );
    }

    return FCursorReply::Cursor( EMouseCursor::Default );
}

FReply
SMetaKeysArea::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    mHoveredKeys = nullptr;

    if( mState == EState::kIdle )
    {
        if( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
        {
            TSharedPtr<FMetaChannel> keys = CreateKeysUnderMouse( MouseEvent );
            if( keys.IsValid() && keys->NumMetaKeys() )
            {
                mState = EState::kPressing;

                check( !mDraggedKeys.IsValid() );
                mDraggedKeys = keys; // Must be done before BeginTransaction

                return FReply::Handled().CaptureMouse( SharedThis( this ) );
            }
        }
    }
    else if( mState == EState::kPressing )
    {
        checkNoEntry();
    }
    else if( mState == EState::kDragging )
    {
        // It may happen when right-clicking during a current dragging
        //checkNoEntry();
    }
    else
    {
        checkNoEntry();
    }

    return SCompoundWidget::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SMetaKeysArea::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    mHoveredKeys = nullptr;

    if( mState == EState::kDragging )
    {
        if( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
        {
            check( HasMouseCapture() );
            check( mDraggedKeys.IsValid() && mDraggedKeys->NumMetaKeys() );

            OnStopDragKeys( mDraggedKeys );

            //-

            mState = EState::kIdle;

            EndTransaction();

            mDraggedKeys = nullptr;

            return FReply::Handled().ReleaseMouseCapture();
        }
    }
    else if( mState == EState::kPressing )
    {
        if( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
        {
            check( HasMouseCapture() );
            check( mDraggedKeys.IsValid() && mDraggedKeys->NumMetaKeys() );

            OnClickKeys( mDraggedKeys );

            //-

            FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
            const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
            ISequencer* sequencer = board_section->GetSequencer().Get();

            FMovieSceneInverseSequenceTransform localToRootTransform = subsection_object->OuterToInnerTransform().Inverse();

            TArray<FFrameNumber> keys;
            mDraggedKeys->GetMetaKeys().GenerateKeyArray( keys ); // Or get the inner time of the first subkey ?

            TOptional<FFrameTime> local_time = localToRootTransform.TryTransformTime( keys[0] );
            // Update the current frame in the sequencer
            if( local_time )
                sequencer->SetLocalTime( *local_time, ESnapTimeMode::STM_Interval );

            //-

            mState = EState::kIdle;

            mDraggedKeys = nullptr;

            return FReply::Handled().ReleaseMouseCapture();
        }
    }
    else if( mState == EState::kIdle )
    {
        if( MouseEvent.GetEffectingButton() == EKeys::RightMouseButton )
        {
            TSharedPtr<FMetaChannel> keys = CreateKeysUnderMouse( MouseEvent );
            if( keys.IsValid() && keys->NumMetaKeys() )
            {
                FMenuBuilder menu_builder( true, nullptr );
                bool is_menu = BuildKeyContextMenu( menu_builder, keys );
                if( is_menu )
                {
                    TSharedPtr<SWidget> menu = menu_builder.MakeWidget();
                    FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
                    FSlateApplication::Get().PushMenu( AsShared(), WidgetPath, menu.ToSharedRef(), MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect( FPopupTransitionEffect::ContextMenu ) );

                    return FReply::Handled();
                }
            }
        }
    }
    else
    {
        checkNoEntry();
    }

    return SCompoundWidget::OnMouseButtonUp( MyGeometry, MouseEvent );
}

void
SMetaKeysArea::ComputeClampRangePreMoveDuringDrag( TSharedPtr<FMetaChannel> iKeys, TRange<FFrameNumber>& oClampRangeInSubsequence ) const
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();

    const FMovieSceneSequenceTransform OuterToInnerTransform = subsection_object->OuterToInnerTransform();

    TRange<FFrameNumber> outerClampRange = subsection_object->GetTrueRange();
    oClampRangeInSubsequence = TRange<FFrameNumber>( ( outerClampRange.GetLowerBoundValue() * OuterToInnerTransform ).FloorToFrame(), ( outerClampRange.GetUpperBoundValue() * OuterToInnerTransform ).FloorToFrame() ); // Let's see if FloorToFrame() of the upper bound value is ok, as (as a true range) it is exclusive

    //UE_LOG( LogTemp, Warning, TEXT( "compute clamp: all: oClampRangeInSubsequence in sequence: %d" ), oClampRangeInSubsequence.GetUpperBoundValue().Value );

    // Must be done after default behavior to set the lower bound
    if( mDragMode == EDragMode::kShiftFromKey )
    {
        oClampRangeInSubsequence.SetUpperBound( TRangeBound<FFrameNumber>::Open() );
        return;
    }
}

FReply
SMetaKeysArea::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( mState == EState::kDragging || mState == EState::kPressing )
    {
        check( HasMouseCapture() );
        check( mDraggedKeys.IsValid() && mDraggedKeys->NumMetaKeys() );

        if( mState == EState::kPressing )
        {
            BeginTransaction( LOCTEXT( "MoveMetaKeyTransaction", "Move Meta Keys" ) );
            mState = EState::kDragging;

            OnStartDragKeys( mDraggedKeys );
        }

        //-

        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        FGeometry geometry;
        FTimeToPixel converter = board_section->ConstructConverterForViewRange( &geometry );
        FFrameTime moved_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

        //---

        const FMovieSceneSequenceTransform OuterToInnerTransform = subsection_object->OuterToInnerTransform();
        FFrameTime inner_moved_frame = moved_frame * OuterToInnerTransform;

        const bool snap = sequencer->GetSequencerSettings()->GetIsSnapEnabled() && sequencer->GetSequencerSettings()->GetForceWholeFrames();
        const FFrameRate inner_tick_resolution = subsection_object->GetSequence()->GetMovieScene()->GetTickResolution();
        const FFrameRate inner_display_rate = subsection_object->GetSequence()->GetMovieScene()->GetDisplayRate();

        //TRange<FFrameNumber> inner_clamp_range;
        //SMetaKeysArea::ComputeClampRangePreMoveDuringDrag( mDraggedKeys, inner_clamp_range ); // Always call the default clamp range to get min/max boundary of the whole board section in the board track
        //TRange<FFrameNumber> inner_clamp_range2;
        //ComputeClampRangePreMoveDuringDrag( mDraggedKeys, inner_clamp_range2 );
        //inner_clamp_range = TRange<FFrameNumber>::Intersection( inner_clamp_range, inner_clamp_range2 );
        TRange<FFrameNumber> inner_clamp_range;
        ComputeClampRangePreMoveDuringDrag( mDraggedKeys, inner_clamp_range );

        //UE_LOG( LogTemp, Warning, TEXT( "OnMouseMove: inner_clamp_range in sequence: %d" ), inner_clamp_range.GetUpperBoundValue().Value );

        FFrameTime local_inner_time = mDraggedKeys->Move( inner_moved_frame, snap, inner_tick_resolution, inner_display_rate, inner_clamp_range );

        FMovieSceneInverseSequenceTransform localToRootTransform = OuterToInnerTransform.Inverse();
        TOptional<FFrameTime> local_time = localToRootTransform.TryTransformTime( local_inner_time );

        OnDragKeys( mDraggedKeys );

        //---

        // Rebuild the full real meta channel
        // This WON'T rebuild the mKeysUnderMouse as it is a copy of the a part of the real meta channel only available during the drag
        RebuildMetaChannel();

        //---

        // Modify all sections where keys have been moved (to force update the viewport)
        for( auto pair : mDraggedKeys->GetMetaKeys() )
        {
            for( const auto& subkey : pair.Value.mSubKeys )
            {
                UMovieSceneSection* section = subkey.mSection.Get();
                if( !section )
                    continue;

                section->TryModify();
            }
        }

        // Update the current frame in the sequencer
        if( local_time )
            sequencer->SetLocalTime( *local_time, ESnapTimeMode::STM_Interval );

        return FReply::Handled();
    }
    else if( mState == EState::kIdle )
    {
        mHoveredKeys = CreateKeysUnderMouse( MouseEvent );

        return SCompoundWidget::OnMouseMove( MyGeometry, MouseEvent );
    }
    else
    {
        checkNoEntry();
    }

    return SCompoundWidget::OnMouseMove( MyGeometry, MouseEvent );
}

void
SMetaKeysArea::OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    // Reset this variable, to not use the old one inside tooltip delegate
    // It may crash because channel proxies and channel handles inside meta channel may be now invalid
    // After doing some process inside popup delegate, it calls iSequencer->Notify() which broadcast message to FKeyThumbnailSection::RebuildKeys() which recompute all proxies
    //
    // the OnMouseEnter() is also called when the popup closes with the mouse over a SMetaKeysArea
    mHoveredKeys = nullptr;
}

void
SMetaKeysArea::OnMouseLeave( const FPointerEvent& MouseEvent ) //override
{
    mHoveredKeys = nullptr;
}

int32
SMetaKeysArea::DrawBackground( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
{
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry( AllottedGeometry.GetLocalSize(), FSlateLayoutTransform() ),
        GetBackgroundBrush(),
        ESlateDrawEffect::None,
        GetBackgroundBrush()->GetTint( InWidgetStyle )
    );

    return LayerId;
}

bool
SMetaKeysArea::ExcludeKey( FFrameNumber iFrameNumber ) const
{
    return false;
}

int32
SMetaKeysArea::DrawKeys( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
{
    TSharedPtr<const FMetaChannel> meta_channel = GetMetaChannel();
    if( !meta_channel.IsValid() )
        return LayerId;

    //---

    if( !mBoardSection.IsValid() )
        return LayerId;

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();
    FTimeToPixel converter = board_section->ConstructConverterForSection( AllottedGeometry );
    FMovieSceneInverseSequenceTransform inner_to_outer_transform = subsection_object->OuterToInnerTransform().Inverse();
    const UMovieScene* movie_scene = subsection_object->GetTypedOuter<UMovieScene>();
    check( movie_scene );

    for( const auto& pair : meta_channel->GetMetaKeys() )
    {
        FFrameNumber time = pair.Key;
        FMetaKey meta_key = pair.Value;
        FKeyDrawParams key_draw_param = meta_key.mMetaKeyDrawParam;

        if( ExcludeKey( time ) )
            continue;

        TOptional<FFrameTime> outer_time = inner_to_outer_transform.TryTransformTime( time );
        if( !outer_time )
            continue;

        double outer_second = FQualifiedFrameTime( *outer_time, movie_scene->GetTickResolution() ).AsSeconds();

        const FVector2D KeySize = SequencerSectionConstants::KeySize;

        static const float BrushBorderWidth = 2.0f;
        const float KeyPositionPx = converter.SecondsToPixel( outer_second );
        const FVector2D KeyTranslation( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f ) ) );
        const FVector2D KeyTranslationBorder( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f - BrushBorderWidth ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f - BrushBorderWidth ) ) );

        key_draw_param.BorderTint = FLinearColor( 0.05f, 0.05f, 0.05f, 1.0f );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize, FSlateLayoutTransform( KeyTranslation ) ),
            key_draw_param.BorderBrush,
            ESlateDrawEffect::None,
            key_draw_param.BorderTint
        );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize - 2.0f * BrushBorderWidth, FSlateLayoutTransform( key_draw_param.FillOffset + KeyTranslationBorder ) ),
            key_draw_param.FillBrush,
            ESlateDrawEffect::None,
            key_draw_param.FillTint
        );
    }

    LayerId++;

    return LayerId;
}

int32
SMetaKeysArea::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    LayerId = DrawBackground( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    LayerId = DrawKeys( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

#undef LOCTEXT_NAMESPACE
