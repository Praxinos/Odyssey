// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionCamera.h"

#include "Brushes/SlateColorBrush.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "SequencerSettings.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
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

FCursorReply
SCinematicBoardSectionCamera::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return FCursorReply::Cursor( EMouseCursor::Default );
}

FReply
SCinematicBoardSectionCamera::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    FGeometry geometry( section->GetSequencer()->GetTopTimeSliderWidget()->GetTickSpaceGeometry() );
    FTimeToPixel converter( geometry, section->GetSequencer()->GetViewRange(), section->GetSequencer()->GetFocusedTickResolution() );
    FFrameTime clicked_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    const FMovieSceneSequenceTransform OuterToInnerTransform = section->GetSubSectionObject().OuterToInnerTransform();
    FFrameNumber inner_frame = ( clicked_frame * OuterToInnerTransform ).GetFrame();

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );

    //---

    check( !mKeyIndexForChannel.Num() );

    TSharedPtr<FMovieSceneChannelProxy> channel_proxy = section->GetCameraTransformChannelProxy();

    TArrayView<FMovieSceneFloatChannel*>          FloatChannels = channel_proxy->GetChannels<FMovieSceneFloatChannel>();
    //TArrayView<const FMovieSceneChannelMetaData>        metaData = channel_proxy->GetMetaData<FMovieSceneFloatChannel>();
    //TArrayView<const TMovieSceneExternalValue<float>>   metaDataExt = channel_proxy->GetAllExtendedEditorData<FMovieSceneFloatChannel>();

    for( int32 Index = 0; Index < FloatChannels.Num(); ++Index )
    {
        TMovieSceneChannelHandle<FMovieSceneFloatChannel> channel_handle = channel_proxy->MakeHandle<FMovieSceneFloatChannel>( Index );

        FMovieSceneFloatChannel* channel = channel_handle.Get();
        if( channel->GetNumKeys() )
        {
            int current_index = channel->GetData().FindKey( inner_frame, HalfKeySizeFrames.CeilToFrame() );
            if( current_index != INDEX_NONE )
                mKeyIndexForChannel.Add( channel_handle, current_index );
        }
    }

    //---

    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseButtonDown" ) );
    return FReply::Handled().CaptureMouse( SharedThis( this ) );
}

FReply
SCinematicBoardSectionCamera::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    mKeyIndexForChannel.Empty();

    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseButtonUp" ) );
    return FReply::Handled().ReleaseMouseCapture();
}

FReply
SCinematicBoardSectionCamera::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( !mKeyIndexForChannel.Num() )
        return SCompoundWidget::OnMouseMove( MyGeometry, MouseEvent );

    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    FGeometry geometry( section->GetSequencer()->GetTopTimeSliderWidget()->GetTickSpaceGeometry() );
    FTimeToPixel converter( geometry, section->GetSequencer()->GetViewRange(), section->GetSequencer()->GetFocusedTickResolution() );
    FFrameTime clicked_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );


    // From ...\Source\Editor\Sequencer\Private\Tools\EditToolDragOperations.cpp -> OnDrag() -> SnapToInterval()
    if( section->GetSequencer()->GetSequencerSettings()->GetIsSnapEnabled() && section->GetSequencer()->GetSequencerSettings()->GetSnapKeyTimesToInterval() )
    {
        // Convert from resolution to DisplayRate, round to frame, then back again. We floor to frames when using the frame block scrubber, and round using the vanilla scrubber
        FFrameTime   DisplayTime = FFrameRate::TransformTime( clicked_frame, section->GetSequencer()->GetFocusedTickResolution(), section->GetSequencer()->GetFocusedDisplayRate() );
        //FFrameNumber PlayIntervalTime = ScrubStyle == ESequencerScrubberStyle::FrameBlock ? DisplayTime.FloorToFrame() : DisplayTime.RoundToFrame();
        FFrameNumber PlayIntervalTime = DisplayTime.FloorToFrame();
        clicked_frame = FFrameRate::TransformTime( PlayIntervalTime, section->GetSequencer()->GetFocusedDisplayRate(), section->GetSequencer()->GetFocusedTickResolution() ).FloorToFrame();
    }


    const FMovieSceneSequenceTransform OuterToInnerTransform = section->GetSubSectionObject().OuterToInnerTransform();
    FFrameNumber inner_frame = ( clicked_frame * OuterToInnerTransform ).GetFrame();

    for( auto& pair : mKeyIndexForChannel )
    {
        TMovieSceneChannelHandle<FMovieSceneFloatChannel> channel_handle = pair.Key;
        FMovieSceneFloatChannel* channel = channel_handle.Get();

        int new_index = channel->GetData().MoveKey( pair.Value, inner_frame );
        pair.Value = new_index;
    }

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

    TSharedPtr<FMovieSceneChannelProxy> channel_proxy = section->GetCameraTransformChannelProxy();

    TArrayView<FMovieSceneFloatChannel*>          FloatChannels = channel_proxy->GetChannels<FMovieSceneFloatChannel>();
    //TArrayView<const FMovieSceneChannelMetaData>        metaData = channel_proxy->GetMetaData<FMovieSceneFloatChannel>();
    //TArrayView<const TMovieSceneExternalValue<float>>   metaDataExt = channel_proxy->GetAllExtendedEditorData<FMovieSceneFloatChannel>();

    TArray<FFrameTime> keys_as_frame;

    for( int32 Index = 0; Index < FloatChannels.Num(); ++Index )
    {
        FMovieSceneFloatChannel* channel = FloatChannels[Index];
        TArrayView<const FFrameNumber> times = channel->GetData().GetTimes();

        for( auto time : times )
            keys_as_frame.AddUnique( time );
    }

    keys_as_frame = SectionsHelpersConvert::InnerToOuter( &section->GetSubSectionObject(), keys_as_frame );
    TArray<double> keys = SectionsHelpersConvert::FrameToSecond( &section->GetSubSectionObject(), keys_as_frame );

    //---

    static const FName CircleKeyBrushName( "Sequencer.KeyCircle" );
    static const FName DiamondKeyBrushName( "Sequencer.KeyDiamond" );
    static const FName SquareKeyBrushName( "Sequencer.KeySquare" );
    static const FName TriangleKeyBrushName( "Sequencer.KeyTriangle" );

    const FSlateBrush* CircleKeyBrush = FEditorStyle::GetBrush( CircleKeyBrushName );
    const FSlateBrush* DiamondKeyBrush = FEditorStyle::GetBrush( DiamondKeyBrushName );
    const FSlateBrush* SquareKeyBrush = FEditorStyle::GetBrush( SquareKeyBrushName );
    const FSlateBrush* TriangleKeyBrush = FEditorStyle::GetBrush( TriangleKeyBrushName );

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();

    for( auto key : keys )
    {
        const FVector2D KeySize = SequencerSectionConstants::KeySize;
        //static const float BrushBorderWidth = 2.0f;
        const float KeyPositionPx = ConstructTimeConverterForSection2( AllottedGeometry, section->GetSubSectionObject() ).SecondsToPixel( key );
        const FVector2D KeyTranslation( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f ) ) );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize, FSlateLayoutTransform( KeyTranslation ) ),
            CircleKeyBrush
        );
    }

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

#undef LOCTEXT_NAMESPACE
