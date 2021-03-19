// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardTrack/MetaChannelProxy.h"

#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneChannelProxy.h"

//---

FMetaFloatChannel::FMetaFloatChannel( const FFrameNumber& iMergeTolerance )
    : TMetaChannel<FMovieSceneFloatChannel, FMovieSceneFloatValue>( iMergeTolerance )
{
}

TSharedPtr<FMetaFloatChannel>
FMetaFloatChannel::CreateFromTime( const FFrameTime& iTime, const FFrameNumber& iTolerance )
{
    TSharedPtr<FMetaFloatChannel> new_meta_channel = MakeShared<FMetaFloatChannel>( mMergeTolerance );

    FillWithTime( iTime, iTolerance, new_meta_channel );

    return new_meta_channel;
}

//---

// From ...\Source\Editor\MovieSceneTools\Private\Channels\BuiltInChannelEditors.cpp
void
DrawKeys( FMovieSceneFloatChannel* Channel, const FKeyHandle& InKeyHandle, FKeyDrawParams& OutKeyDrawParams )
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

void
FMetaFloatChannel::BuildDrawKeys()
{
    for( auto& pair : mMetaKeys )
    {
        FMetaKey& meta_key = pair.Value;

        for( auto& sub_key : meta_key.mSubKeys )
        {
            TMovieSceneChannelHandle<FMovieSceneFloatChannel> channel_handle = sub_key.mChannelHandle.Cast<FMovieSceneFloatChannel>();
            FMovieSceneFloatChannel* float_channel = channel_handle.Get();
            if( !float_channel )
                continue;

            DrawKeys( float_channel, sub_key.mKeyHandle, sub_key.mKeyDrawParam );
        }
    }
}
