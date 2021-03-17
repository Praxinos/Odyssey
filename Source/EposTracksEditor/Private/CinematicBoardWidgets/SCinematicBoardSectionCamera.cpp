// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionCamera.h"

#include "Brushes/SlateColorBrush.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "KeyDrawParams.h"
#include "SequencerSettings.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/CinematicBoardSectionHelpers.h"
#include "CinematicBoardTrack/MetaChannelProxy.h"
#include "Helpers/SectionsHelpersConvert.h"

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

TSharedPtr<FMetaChannelProxy>
SCinematicBoardSectionCamera::GetKeysUnderMouse( const FPointerEvent& MouseEvent ) const
{
    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    const FMovieSceneSequenceTransform OuterToInnerTransform = section->GetSubSectionObject().OuterToInnerTransform();

    FGeometry geometry( section->GetSequencer()->GetTopTimeSliderWidget()->GetTickSpaceGeometry() );
    FTimeToPixel converter( geometry, section->GetSequencer()->GetViewRange(), section->GetSequencer()->GetFocusedTickResolution() );
    FFrameTime clicked_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    TRange<FFrameNumber> range( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );

    //---

    FFrameTime inner_clicked_frame = clicked_frame * OuterToInnerTransform;

    //---

    TSharedPtr<FMovieSceneChannelProxy> channel_proxy = section->GetCameraTransformChannelProxy();
    TSharedPtr<FMetaChannelProxy> meta_channel_proxy = CinematicBoardSectionKeysHelpers::BuildCameraTransformMetaChannelProxy( channel_proxy, range );

    for( auto& pair : meta_channel_proxy->mMetaKeys )
    {
        FFrameNumber time = pair.Key;
        FMetaKey& meta_key = pair.Value;

        for( auto& sub_key : meta_key.mSubKeys )
        {
            FFrameNumber key_time;
            sub_key.mChannelHandle.Get()->GetKeyTime( sub_key.mKeyHandle, key_time );
            sub_key.mOffset = inner_clicked_frame - key_time;
        }
    }

    return meta_channel_proxy;
}

//---

FCursorReply
SCinematicBoardSectionCamera::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    TSharedPtr<FMetaChannelProxy> proxy = GetKeysUnderMouse( CursorEvent );

    if( proxy->mMetaKeys.Num() )
        return FCursorReply::Cursor( EMouseCursor::CardinalCross );

    return FCursorReply::Cursor( EMouseCursor::Default );
}

FReply
SCinematicBoardSectionCamera::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    check( !mKeysUnderMouse.IsValid() );

    mKeysUnderMouse = GetKeysUnderMouse( MouseEvent );

    //---

    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseButtonDown" ) );
    return FReply::Handled().CaptureMouse( SharedThis( this ) );
}

FReply
SCinematicBoardSectionCamera::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    mKeysUnderMouse = nullptr;

    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseButtonUp" ) );
    return FReply::Handled().ReleaseMouseCapture();
}

FReply
SCinematicBoardSectionCamera::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( !HasMouseCapture() )
    {
        //return FReply::Handled();
        return SCompoundWidget::OnMouseMove( MyGeometry, MouseEvent );
    }

    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    FGeometry geometry( section->GetSequencer()->GetTopTimeSliderWidget()->GetTickSpaceGeometry() );
    FTimeToPixel converter( geometry, section->GetSequencer()->GetViewRange(), section->GetSequencer()->GetFocusedTickResolution() );
    FFrameTime moved_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    //---

    const FMovieSceneSequenceTransform OuterToInnerTransform = section->GetSubSectionObject().OuterToInnerTransform();
    const FMovieSceneSequenceTransform InnerToOuterTransform = section->GetSubSectionObject().OuterToInnerTransform().InverseLinearOnly();
    FFrameTime inner_moved_frame = moved_frame * OuterToInnerTransform;

    // For the moment should always be the case
    check( mKeysUnderMouse->mMetaKeys.Num() == 1 );

    for( auto& pair : mKeysUnderMouse->mMetaKeys )
    {
        for( auto& sub_key : pair.Value.mSubKeys )
        {
            TMovieSceneChannelHandle<FMovieSceneFloatChannel> channel_handle = sub_key.mChannelHandle.Cast<FMovieSceneFloatChannel>();
            FKeyHandle& key_handle = sub_key.mKeyHandle;
            FFrameTime offset = sub_key.mOffset;

            FMovieSceneFloatChannel* channel = channel_handle.Get();
            if( !channel )
                continue;

            TMovieSceneChannelData<FMovieSceneFloatValue> channel_data = channel->GetData();
            int32 key_index = channel_data.GetIndex( key_handle );

            FFrameNumber inner_key_frame = channel_data.GetTimes()[key_index];
            FFrameTime inner_moved_key_frame = inner_moved_frame - offset;

            // From ...\Source\Editor\Sequencer\Private\Tools\EditToolDragOperations.cpp -> OnDrag() -> SnapToInterval()
            if( section->GetSequencer()->GetSequencerSettings()->GetIsSnapEnabled() && section->GetSequencer()->GetSequencerSettings()->GetSnapKeyTimesToInterval() )
            {
                FFrameRate inner_tick_resolution = section->GetSubSectionObject().GetSequence()->GetMovieScene()->GetTickResolution();
                FFrameRate inner_display_rate = section->GetSubSectionObject().GetSequence()->GetMovieScene()->GetDisplayRate();

                // Convert from resolution to DisplayRate, round to frame, then back again. We floor to frames when using the frame block scrubber, and round using the vanilla scrubber
                FFrameTime   DisplayTime = FFrameRate::TransformTime( inner_moved_key_frame, inner_tick_resolution, inner_display_rate );
                //FFrameNumber PlayIntervalTime = ScrubStyle == ESequencerScrubberStyle::FrameBlock ? DisplayTime.FloorToFrame() : DisplayTime.RoundToFrame();
                FFrameNumber PlayIntervalTime = DisplayTime.FloorToFrame();
                inner_moved_key_frame = FFrameRate::TransformTime( PlayIntervalTime, inner_display_rate, inner_tick_resolution ).FloorToFrame();
            }


            int32 new_key_index = channel_data.MoveKey( key_index, inner_moved_key_frame.GetFrame() );
            key_handle = channel_data.GetHandle( new_key_index );
        }
    }

    section->ReBuildCameraTransformMetaKeys();

    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseMove" ) );
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

static
FTimeToPixel
ConstructTimeConverterForSection2( const FGeometry& InSectionGeometry, const UMovieSceneSection& InSection )
{
    FFrameRate     TickResolution = InSection.GetTypedOuter<UMovieScene>()->GetTickResolution();
    double         LowerTime = InSection.GetInclusiveStartFrame() / TickResolution;
    double         UpperTime = InSection.GetExclusiveEndFrame() / TickResolution;

    return FTimeToPixel( InSectionGeometry, TRange<double>( LowerTime, UpperTime ), TickResolution );
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

    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    //TSharedPtr<FMovieSceneChannelProxy> channel_proxy = section->GetCameraTransformChannelProxy();
    TSharedPtr<FMetaChannelProxy> meta_channel_proxy = section->GetCameraTransformMetaChannelProxy();

    //---

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();

    for( const auto& pair : meta_channel_proxy->mMetaKeys )
    {
        FFrameNumber time = pair.Key;
        FMetaKey meta_key = pair.Value;
        FKeyDrawParams key_draw_param = meta_key.mMetaKeyDrawParam;

        FFrameTime outer_time = SectionsHelpersConvert::InnerToOuter( &section->GetSubSectionObject(), time );
        double outer_second = SectionsHelpersConvert::FrameToSecond( &section->GetSubSectionObject(), outer_time );

        const FVector2D KeySize = SequencerSectionConstants::KeySize;

        static const float BrushBorderWidth = 2.0f;
        const float KeyPositionPx = ConstructTimeConverterForSection2( AllottedGeometry, section->GetSubSectionObject() ).SecondsToPixel( outer_second );
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
