// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionCamera.h"

#include "Brushes/SlateColorBrush.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "KeyDrawParams.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "SequencerSettings.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MetaChannelProxy.h"
#include "EposSequenceHelpers.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionCamera"

//---

void
SCinematicBoardSectionCamera::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    ChildSlot
    [
        SNew( SBox )
        //[
        //    SNew( STextBlock )
        //    .Text( GetBindingName() ) )
        //]
    ];
}

FVector2D
SCinematicBoardSectionCamera::ComputeDesiredSize( float ) const //override
{
    FVector2D size = GetDesiredSize();
    size.Y = SequencerSectionConstants::DefaultSectionHeight + 5.f;

    return size;
}

//---

TSharedPtr<FMetaFloatChannel>
SCinematicBoardSectionCamera::GetKeysUnderMouse( const FPointerEvent& MouseEvent ) const
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

    TSharedPtr<FMetaFloatChannel> meta_channel = board_section->GetCameraTransformMetaChannel();
    if( !meta_channel )
        return nullptr;

    return meta_channel->CreateFromTime( inner_clicked_frame, inner_tolerance );
}

void
SCinematicBoardSectionCamera::BeginTransaction( const FText& iTransactionDesc ) // From FEditToolDragOperation::BeginTransaction#95
{
    // Begin an editor transaction and mark the section as transactional so it's state will be saved
    mTransaction.Reset( new FScopedTransaction( iTransactionDesc ) );

    //---

    for( auto pair : mKeysUnderMouse->GetMetaKeys() )
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
SCinematicBoardSectionCamera::EndTransaction()
{
    mTransaction.Reset();

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    ISequencer*             sequencer = board_section->GetSequencer().Get();

    sequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

void
SCinematicBoardSectionCamera::BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder )
{
    auto DeleteKey = [=]( TSharedPtr<FMetaFloatChannel> iKeysUnderMouse )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        const FScopedTransaction transaction( LOCTEXT( "DeleteCameraKeys", "Delete camera keys" ) );

        for( auto pair : iKeysUnderMouse->GetMetaKeys() )
        {
            for( const auto& subkey : pair.Value.mSubKeys )
            {
                BoardSequenceTools::DeleteCameraKey( sequencer, *subsection_object, subkey.mSection.Get(), subkey.mChannelHandle, subkey.mKeyHandle );
            }
        }
    };

    auto CanDeleteKey = [=]( TSharedPtr<FMetaFloatChannel> iKeysUnderMouse ) -> bool
    {
        return true;
    };

    //-

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "delete-camera-key-label", "Delete" ), //TODO: find a way to know the number of "symbolic" keys deleted, 1 symbolic key should represent a key at the same time for the 9 (maybe more or less) channels
                                LOCTEXT( "delete-camera-key-tooltip", "Delete the current key" ),
                                FSlateIcon(),
                                FUIAction( FExecuteAction::CreateLambda( DeleteKey, mKeysUnderMouse ),
                                           FCanExecuteAction::CreateLambda( CanDeleteKey, mKeysUnderMouse ) ) );


}

//---

FCursorReply
SCinematicBoardSectionCamera::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    TSharedPtr<FMetaFloatChannel> meta_channel = GetKeysUnderMouse( CursorEvent );

    if( meta_channel.IsValid() && meta_channel->NumMetaKeys() )
        return FCursorReply::Cursor( EMouseCursor::CardinalCross );

    return FCursorReply::Cursor( EMouseCursor::Default );
}

FReply
SCinematicBoardSectionCamera::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    check( !mKeysUnderMouse.IsValid() );

    mKeysUnderMouse = GetKeysUnderMouse( MouseEvent );

    if( !mKeysUnderMouse.IsValid() || !mKeysUnderMouse->NumMetaKeys() )
    {
        mKeysUnderMouse = nullptr; // doesn't go inside OnMouseButtonUp(), so reset it here

        return SCompoundWidget::OnMouseButtonDown( MyGeometry, MouseEvent );
    }

    //---

    if( MouseEvent.GetEffectingButton() == EKeys::RightMouseButton )
    {
        FMenuBuilder menu_builder( true, nullptr );
        BuildKeyContextMenu( menu_builder );

        TSharedPtr<SWidget> menu = menu_builder.MakeWidget();
        FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
        FSlateApplication::Get().PushMenu( AsShared(), WidgetPath, menu.ToSharedRef(), MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect( FPopupTransitionEffect::ContextMenu ) );

        mKeysUnderMouse = nullptr; // doesn't go inside OnMouseButtonUp(), so reset it here

        return FReply::Handled();
    }

    BeginTransaction( LOCTEXT( "MoveCameraKeyTransaction", "Move Camera Keys" ) );

    return FReply::Handled().CaptureMouse( SharedThis( this ) );
}

FReply
SCinematicBoardSectionCamera::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    EndTransaction();

    mKeysUnderMouse = nullptr;

    return FReply::Handled().ReleaseMouseCapture();
}

FReply
SCinematicBoardSectionCamera::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( !HasMouseCapture() || !mKeysUnderMouse.IsValid() || !mKeysUnderMouse->NumMetaKeys() )
    {
        //return FReply::Handled();
        return SCompoundWidget::OnMouseMove( MyGeometry, MouseEvent );
    }

    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();
    ISequencer*                     sequencer = board_section->GetSequencer().Get();

    FGeometry geometry;
    FTimeToPixel converter = board_section->ConstructConverterForViewRange( &geometry );
    FFrameTime moved_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    //---

    const FMovieSceneSequenceTransform OuterToInnerTransform = subsection_object->OuterToInnerTransform();
    FFrameTime inner_moved_frame = moved_frame * OuterToInnerTransform;

    // For the moment, this should always be the case (until meta keys selection)
    check( mKeysUnderMouse->NumMetaKeys() == 1 );

    const bool snap = sequencer->GetSequencerSettings()->GetIsSnapEnabled() && sequencer->GetSequencerSettings()->GetSnapKeyTimesToInterval();
    const FFrameRate inner_tick_resolution = subsection_object->GetSequence()->GetMovieScene()->GetTickResolution();
    const FFrameRate inner_display_rate = subsection_object->GetSequence()->GetMovieScene()->GetDisplayRate();

    FFrameTime local_inner_time = mKeysUnderMouse->Move( inner_moved_frame, snap, inner_tick_resolution, inner_display_rate );
    FFrameTime local_time = local_inner_time * OuterToInnerTransform.InverseLinearOnly();

    //---

    // Rebuild the full real meta channel
    // This WON'T rebuild the mKeysUnderMouse as it is a copy of the a part of the real meta channel only available during the drag
    board_section->ReBuildCameraTransformMetaChannel();

    //---

    // Modify all sections where keys have been moved (to force update the viewport)
    for( auto pair : mKeysUnderMouse->GetMetaKeys() )
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
    if( sequencer->GetSequencerSettings()->GetIsSnapEnabled() )
    {
        FFrameRate LocalResolution = sequencer->GetFocusedTickResolution();
        FFrameRate LocalDisplayRate = sequencer->GetFocusedDisplayRate();
        local_time = FFrameRate::TransformTime( FFrameRate::TransformTime( local_time, LocalResolution, LocalDisplayRate ).FloorToFrame(), LocalDisplayRate, LocalResolution );
    }
    sequencer->SetLocalTime( local_time );

    return FReply::Handled();
}

void
SCinematicBoardSectionCamera::OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseEnter" ) );
}

void
SCinematicBoardSectionCamera::OnMouseLeave( const FPointerEvent& MouseEvent ) //override
{
    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseLeave" ) );
}

int32
SCinematicBoardSectionCamera::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .15f, .06f, .14f ) );

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry( AllottedGeometry.GetLocalSize(), FSlateLayoutTransform() ),
        &background_brush,
        ESlateDrawEffect::None,
        background_brush.GetTint( InWidgetStyle )
    );

    if( !mBoardSection.IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();

    //TSharedPtr<FMovieSceneChannelProxy> channel_proxy = section->GetCameraTransformChannelProxy();
    TSharedPtr<FMetaFloatChannel> meta_channel = board_section->GetCameraTransformMetaChannel();

    if( !meta_channel.IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();
    FTimeToPixel converter = board_section->ConstructConverterForSection( AllottedGeometry );
    const FMovieSceneSequenceTransform inner_to_outer_transform = subsection_object->OuterToInnerTransform().InverseLinearOnly();
    const UMovieScene* movie_scene = subsection_object->GetTypedOuter<UMovieScene>();
    check( movie_scene );

    for( const auto& pair : meta_channel->GetMetaKeys() )
    {
        FFrameNumber time = pair.Key;
        FMetaKey meta_key = pair.Value;
        FKeyDrawParams key_draw_param = meta_key.mMetaKeyDrawParam;

        FFrameTime outer_time = time * inner_to_outer_transform;
        double outer_second = FQualifiedFrameTime( outer_time, movie_scene->GetTickResolution() ).AsSeconds();

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

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

#undef LOCTEXT_NAMESPACE
