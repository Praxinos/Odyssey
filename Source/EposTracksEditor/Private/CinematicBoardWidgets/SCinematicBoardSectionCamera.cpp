// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionCamera.h"

#include "Brushes/SlateColorBrush.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "KeyDrawParams.h"
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

//---

SCinematicBoardSectionCamera::FMapFloatChannelHandleToKeyIndex
SCinematicBoardSectionCamera::GetKeysUnderCursor( const FPointerEvent& MouseEvent ) const
{
    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    FGeometry geometry( section->GetSequencer()->GetTopTimeSliderWidget()->GetTickSpaceGeometry() );
    FTimeToPixel converter( geometry, section->GetSequencer()->GetViewRange(), section->GetSequencer()->GetFocusedTickResolution() );
    FFrameTime clicked_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    const FMovieSceneSequenceTransform OuterToInnerTransform = section->GetSubSectionObject().OuterToInnerTransform();
    FFrameNumber inner_frame = ( clicked_frame * OuterToInnerTransform ).GetFrame();

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );

    //---

    TSharedPtr<FMovieSceneChannelProxy> channel_proxy = section->GetCameraTransformChannelProxy();

    TArrayView<FMovieSceneFloatChannel*>                FloatChannels = channel_proxy->GetChannels<FMovieSceneFloatChannel>();
    //TArrayView<const FMovieSceneChannelMetaData>        metaData = channel_proxy->GetMetaData<FMovieSceneFloatChannel>();
    //TArrayView<const TMovieSceneExternalValue<float>>   metaDataExt = channel_proxy->GetAllExtendedEditorData<FMovieSceneFloatChannel>();

    FMapFloatChannelHandleToKeyIndex map;

    for( int32 Index = 0; Index < FloatChannels.Num(); ++Index )
    {
        TMovieSceneChannelHandle<FMovieSceneFloatChannel> channel_handle = channel_proxy->MakeHandle<FMovieSceneFloatChannel>( Index );

        FMovieSceneFloatChannel* channel = channel_handle.Get();
        if( channel->GetNumKeys() )
        {
            int32 current_index = channel->GetData().FindKey( inner_frame, HalfKeySizeFrames.CeilToFrame() );
            if( current_index != INDEX_NONE )
                map.Add( channel_handle, current_index ); // Store only 1 index key per channel
        }
    }

    return map;
}

//---

FCursorReply
SCinematicBoardSectionCamera::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    FMapFloatChannelHandleToKeyIndex map = GetKeysUnderCursor( CursorEvent );

    if( map.Num() )
        return FCursorReply::Cursor( EMouseCursor::CardinalCross );

    return FCursorReply::Cursor( EMouseCursor::Default );
}

FReply
SCinematicBoardSectionCamera::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    check( !mKeyIndexForChannel.Num() );

    mKeyIndexForChannel = GetKeysUnderCursor( MouseEvent );

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
    if( !HasMouseCapture() )
    {
        //return FReply::Handled();
        return SCompoundWidget::OnMouseMove( MyGeometry, MouseEvent );
    }

    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    FGeometry geometry( section->GetSequencer()->GetTopTimeSliderWidget()->GetTickSpaceGeometry() );
    FTimeToPixel converter( geometry, section->GetSequencer()->GetViewRange(), section->GetSequencer()->GetFocusedTickResolution() );
    FFrameTime moved_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );


    // From ...\Source\Editor\Sequencer\Private\Tools\EditToolDragOperations.cpp -> OnDrag() -> SnapToInterval()
    if( section->GetSequencer()->GetSequencerSettings()->GetIsSnapEnabled() && section->GetSequencer()->GetSequencerSettings()->GetSnapKeyTimesToInterval() )
    {
        // Convert from resolution to DisplayRate, round to frame, then back again. We floor to frames when using the frame block scrubber, and round using the vanilla scrubber
        FFrameTime   DisplayTime = FFrameRate::TransformTime( moved_frame, section->GetSequencer()->GetFocusedTickResolution(), section->GetSequencer()->GetFocusedDisplayRate() );
        //FFrameNumber PlayIntervalTime = ScrubStyle == ESequencerScrubberStyle::FrameBlock ? DisplayTime.FloorToFrame() : DisplayTime.RoundToFrame();
        FFrameNumber PlayIntervalTime = DisplayTime.FloorToFrame();
        moved_frame = FFrameRate::TransformTime( PlayIntervalTime, section->GetSequencer()->GetFocusedDisplayRate(), section->GetSequencer()->GetFocusedTickResolution() ).FloorToFrame();
    }


    const FMovieSceneSequenceTransform OuterToInnerTransform = section->GetSubSectionObject().OuterToInnerTransform();
    FFrameNumber inner_frame = ( moved_frame * OuterToInnerTransform ).GetFrame();

    for( auto& pair : mKeyIndexForChannel )
    {
        TMovieSceneChannelHandle<FMovieSceneFloatChannel> channel_handle = pair.Key;
        FMovieSceneFloatChannel* channel = channel_handle.Get();

        int32 new_index = channel->GetData().MoveKey( pair.Value, inner_frame );
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


// From ...\Source\Editor\MovieSceneTools\Private\Channels\BuiltInChannelEditors.cpp
static
void
DrawKeys( FMovieSceneFloatChannel* Channel, const FKeyHandle& InKeyHandle, const UMovieSceneSection* InOwner, FKeyDrawParams& OutKeyDrawParams )
//DrawKeys( FMovieSceneFloatChannel* Channel, TArrayView<const FKeyHandle> InKeyHandles, const UMovieSceneSection* InOwner, TArrayView<FKeyDrawParams> OutKeyDrawParams )
{
    static const FName CircleKeyBrushName( "Sequencer.KeyCircle" );
    static const FName DiamondKeyBrushName( "Sequencer.KeyDiamond" );
    static const FName SquareKeyBrushName( "Sequencer.KeySquare" );
    static const FName TriangleKeyBrushName( "Sequencer.KeyTriangle" );

    const FSlateBrush* CircleKeyBrush = FEditorStyle::GetBrush( CircleKeyBrushName );
    const FSlateBrush* DiamondKeyBrush = FEditorStyle::GetBrush( DiamondKeyBrushName );
    const FSlateBrush* SquareKeyBrush = FEditorStyle::GetBrush( SquareKeyBrushName );
    const FSlateBrush* TriangleKeyBrush = FEditorStyle::GetBrush( TriangleKeyBrushName );

    TMovieSceneChannelData<FMovieSceneFloatValue> ChannelData = Channel->GetData();
    TArrayView<const FMovieSceneFloatValue> Values = ChannelData.GetValues();

    FKeyDrawParams TempParams;
    TempParams.BorderBrush = TempParams.FillBrush = DiamondKeyBrush;

    //for( int32 Index = 0; Index < InKeyHandles.Num(); ++Index )
    {
        //FKeyHandle Handle = InKeyHandles[Index];

        const int32 KeyIndex = ChannelData.GetIndex( InKeyHandle );
        //const int32 KeyIndex = ChannelData.GetIndex( Handle );

        ERichCurveInterpMode InterpMode = KeyIndex == INDEX_NONE ? RCIM_None : Values[KeyIndex].InterpMode.GetValue();
        ERichCurveTangentMode TangentMode = KeyIndex == INDEX_NONE ? RCTM_None : Values[KeyIndex].TangentMode.GetValue();

        TempParams.FillOffset = FVector2D( 0.f, 0.f );

        switch( InterpMode )
        {
            case RCIM_Linear:
                TempParams.BorderBrush = TempParams.FillBrush = TriangleKeyBrush;
                TempParams.FillTint = FLinearColor( 0.0f, 0.617f, 0.449f, 1.0f ); // blueish green
                TempParams.FillOffset = FVector2D( 0.0f, 1.0f );
                break;

            case RCIM_Constant:
                TempParams.BorderBrush = TempParams.FillBrush = SquareKeyBrush;
                TempParams.FillTint = FLinearColor( 0.0f, 0.445f, 0.695f, 1.0f ); // blue
                break;

            case RCIM_Cubic:
                TempParams.BorderBrush = TempParams.FillBrush = CircleKeyBrush;

                switch( TangentMode )
                {
                    case RCTM_Auto:  TempParams.FillTint = FLinearColor( 0.972f, 0.2f, 0.2f, 1.0f );     break; // vermillion
                    case RCTM_Break: TempParams.FillTint = FLinearColor( 0.336f, 0.703f, 0.5f, 0.91f );  break; // sky blue
                    case RCTM_User:  TempParams.FillTint = FLinearColor( 0.797f, 0.473f, 0.5f, 0.652f ); break; // reddish purple
                    default:         TempParams.FillTint = FLinearColor( 0.75f, 0.75f, 0.75f, 1.0f );    break; // light gray
                }
                break;

            default:
                TempParams.BorderBrush = TempParams.FillBrush = DiamondKeyBrush;
                TempParams.FillTint = FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f ); // white
                break;
        }

        OutKeyDrawParams = TempParams;
        //OutKeyDrawParams[Index] = TempParams;
    }
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

    static const FSlateBrush* PartialKeyBrush = FEditorStyle::GetBrush( "Sequencer.PartialKey" );

    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    TSharedPtr<FMovieSceneChannelProxy> channel_proxy = section->GetCameraTransformChannelProxy();

    TArrayView<FMovieSceneFloatChannel*>          FloatChannels = channel_proxy->GetChannels<FMovieSceneFloatChannel>();
    //TArrayView<const FMovieSceneChannelMetaData>        metaData = channel_proxy->GetMetaData<FMovieSceneFloatChannel>();
    //TArrayView<const TMovieSceneExternalValue<float>>   metaDataExt = channel_proxy->GetAllExtendedEditorData<FMovieSceneFloatChannel>();

    struct FKeyParams
    {
        FFrameTime  mInnerFrameTime;
        FFrameTime  mOuterFrameTime;
        double      mOuterFrameSecond;

        FKeyDrawParams mKeyDrawParams;
    };
    TArray<FKeyParams> key_params;

    for( int32 Index = 0; Index < FloatChannels.Num(); ++Index )
    {
        FMovieSceneFloatChannel* channel = FloatChannels[Index];
        TArrayView<const FFrameNumber> times = channel->GetData().GetTimes();

        for( int ti = 0; ti < times.Num(); ti++ )
        {
            FFrameNumber time = times[ti];

            FKeyHandle key_handle = channel->GetData().GetHandle( ti );
            FKeyDrawParams key_draw_param;
            DrawKeys( channel, key_handle, &section->GetSubSectionObject(), key_draw_param );

            FKeyParams* found_key_param = key_params.FindByPredicate( [&]( const FKeyParams& iElement ){ return iElement.mInnerFrameTime == time; } );
            if( found_key_param )
            {
                // If there is already a key at the same time, check if they share both the same drawing params (circle/square/...)
                // If they are different, use the partial key drawing
                if( found_key_param->mKeyDrawParams != key_draw_param )
                {
                    found_key_param->mKeyDrawParams.BorderBrush = PartialKeyBrush;
                    found_key_param->mKeyDrawParams.FillBrush = PartialKeyBrush;
                    found_key_param->mKeyDrawParams.FillOffset = FVector2D( 0.f, 0.f );
                    found_key_param->mKeyDrawParams.FillTint = FLinearColor::White;
                    found_key_param->mKeyDrawParams.BorderTint = FLinearColor::White;
                }

                // Otherwise, nothing to do
            }
            else
            {
                // If no key at the time, just add a new one
                FKeyParams key_param;
                key_param.mInnerFrameTime = time;
                key_param.mOuterFrameTime = SectionsHelpersConvert::InnerToOuter( &section->GetSubSectionObject(), key_param.mInnerFrameTime );
                key_param.mOuterFrameSecond = SectionsHelpersConvert::FrameToSecond( &section->GetSubSectionObject(), key_param.mOuterFrameTime );
                key_param.mKeyDrawParams = key_draw_param;

                key_params.Add( key_param );
            }
        }
    }

    //---

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();

    for( auto key_param : key_params )
    {
        const FVector2D KeySize = SequencerSectionConstants::KeySize;

        static const float BrushBorderWidth = 2.0f;
        const float KeyPositionPx = ConstructTimeConverterForSection2( AllottedGeometry, section->GetSubSectionObject() ).SecondsToPixel( key_param.mOuterFrameSecond );
        const FVector2D KeyTranslation( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f ) ) );
        const FVector2D KeyTranslationBorder( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f - BrushBorderWidth ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f - BrushBorderWidth ) ) );

        key_param.mKeyDrawParams.BorderTint = FLinearColor( 0.05f, 0.05f, 0.05f, 1.0f );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize, FSlateLayoutTransform( KeyTranslation ) ),
            key_param.mKeyDrawParams.BorderBrush,
            ESlateDrawEffect::None,
            key_param.mKeyDrawParams.BorderTint
        );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize - 2.0f * BrushBorderWidth, FSlateLayoutTransform( key_param.mKeyDrawParams.FillOffset + KeyTranslationBorder ) ),
            key_param.mKeyDrawParams.FillBrush,
            ESlateDrawEffect::None,
            key_param.mKeyDrawParams.FillTint
        );
    }

    LayerId++;

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

#undef LOCTEXT_NAMESPACE
