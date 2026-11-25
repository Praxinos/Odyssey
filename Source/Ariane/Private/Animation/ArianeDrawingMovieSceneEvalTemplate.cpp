// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeDrawingMovieSceneEvalTemplate.h"
#include "ArianeDrawing.h"
#include "ArianeGroup.h"
#include "ArianeLayerVector.h"
#include "Evaluation/MovieSceneAnimTypeID.h"

#include "Evaluation/MovieScenePreAnimatedState.h"

// 1. The token: stores the original state and knows how to restore it
struct FArianeDrawingPreAnimatedToken : IMovieScenePreAnimatedToken
{
    // TStrongObjectPtr prevents Garbage collection, as the key is the only one to store the former Drawing
    TStrongObjectPtr<UArianeDrawing> OldDrawing;

    FArianeDrawingPreAnimatedToken(UArianeDrawing* InDrawing) : OldDrawing(InDrawing) {}

    virtual void RestoreState(UObject& Object, const UE::MovieScene::FRestoreStateParams& Params) override
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(&Object);
        if (VectorLayer)
        {
            // Restore the image saved before the animation
            VectorLayer->SetDrawing(OldDrawing.Get());

            VectorLayer->GetDrawing()->GetRootGroup()->UpdateTransform();
            VectorLayer->Update( false );
        }
    }
};

// 2. The producer: called by Unreal to create the token above
struct FArianeDrawingPreAnimatedTokenProducer : IMovieScenePreAnimatedTokenProducer
{
    virtual IMovieScenePreAnimatedTokenPtr CacheExistingState(UObject& Object) const override
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(&Object);
        if (VectorLayer)
        {
            // Capture the current image (the map's base image)
            return FArianeDrawingPreAnimatedToken(VectorLayer->GetDrawing());
        }
        return IMovieScenePreAnimatedTokenPtr();
    }
};


// Declare the unique identifier used to restore the ArianeDrawing
static
FMovieSceneAnimTypeID GetArianeDrawingAnimTypeID()
{
    static FMovieSceneAnimTypeID TypeID = FMovieSceneAnimTypeID::Unique();

    return TypeID;
}

FArianeDrawingExecutionToken::FArianeDrawingExecutionToken( const FArianeDrawingKeyData* InKeyData
                                                      , const FArianeDrawingKeyData* InNextKeyData
                                                      , float InT
                                                      , const FMovieSceneEvaluationOperand& InOperand )
    : KeyData(InKeyData)
    , NextKeyData(InNextKeyData)
    , T ( InT )
    , StoredOperand( InOperand )
{
}

void FArianeDrawingExecutionToken::Execute( const FMovieSceneContext& Context
                                        , const FMovieSceneEvaluationOperand& Operand
                                        , FPersistentEvaluationData& PersistentData
                                        , IMovieScenePlayer& Player )
{
    // 1. Resolve the object bound to the track
    TArrayView<TWeakObjectPtr<UObject>> BoundObjects = Player.FindBoundObjects( StoredOperand.ObjectBindingID, StoredOperand.SequenceID );

    if ( BoundObjects.Num() > 0 )
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>( BoundObjects[0].Get() );

        if( VectorLayer )
        {
            EMovieScenePlayerStatus::Type PlaybackStatus = Player.GetPlaybackStatus();

            // THE FIX: save the current state (the default image) before modifying it.
            // Uses the Player's 'PreAnimatedState'.
            Player.SavePreAnimatedState( *VectorLayer
                                       , GetArianeDrawingAnimTypeID()
                                       , FArianeDrawingPreAnimatedTokenProducer () );

            if ( KeyData->Drawing )
            {
                bool bInteractive =( ( PlaybackStatus == EMovieScenePlayerStatus::Type::Scrubbing )
                                  || ( PlaybackStatus == EMovieScenePlayerStatus::Type::Playing   ) ) ? true : false;

                VectorLayer->SetDrawing( KeyData->Drawing, bInteractive ? false : true );

                VectorLayer->GetDrawing()->Animate( KeyData, NextKeyData, T );

                VectorLayer->GetDrawing()->GetRootGroup()->UpdateTransform();
                VectorLayer->Update( bInteractive );
            }
        }
    }
}

FArianeDrawingMovieSceneEvalTemplate::FArianeDrawingMovieSceneEvalTemplate()
    : Section ( nullptr )
{
}

FArianeDrawingMovieSceneEvalTemplate::FArianeDrawingMovieSceneEvalTemplate( const UArianeDrawingMovieSceneSection* InSection )
    : FMovieSceneEvalTemplate()
    , Section( InSection )
{
}

UScriptStruct& FArianeDrawingMovieSceneEvalTemplate::GetScriptStructImpl() const
{
    return *StaticStruct();
}

void
FArianeDrawingMovieSceneEvalTemplate::Evaluate( const FMovieSceneEvaluationOperand& Operand
                                            , const FMovieSceneContext& Context
                                            , const FPersistentEvaluationData& PersistentData
                                            , FMovieSceneExecutionTokens& ExecutionTokens ) const
{
    // Get the source section bound to this template
    const UArianeDrawingMovieSceneSection* ArianeSection = Cast<UArianeDrawingMovieSceneSection>(GetSourceSection());
    if (!ArianeSection) return;

    // 1. Get the Sequencer's current time
    const FFrameTime CurrentTime = Context.GetTime();

    // 2. Get access to the channel's data
    // Note: assumes the channel 'MyChannel' was stored in the template when it was compiled
    TMovieSceneChannelData<const FArianeDrawingKeyData> ChannelData = Section->DrawingChannel.GetData();

    // The array of times managed by the channel
    TArrayView<const FFrameNumber> Times = ChannelData.GetTimes();
    // The array of data structures
    TArrayView<const FArianeDrawingKeyData> Values = ChannelData.GetValues();

    const FArianeDrawingKeyData* KeyDataA = nullptr;
    const FArianeDrawingKeyData* KeyDataB = nullptr;
    float Alpha = 0.0f;

    if (Times.Num() > 0)
    {
        // 3. Binary search on the native FFrameNumber array
        int32 Index = Algo::UpperBound(Times, CurrentTime.GetFrame()) - 1;
        Index = FMath::Clamp(Index, 0, Times.Num() - 1);

        // Current key (lower bound)
        KeyDataA = &Values[Index];

        // 4. Next key (upper bound), if any, to compute the Lerp
        if (Index + 1 < Times.Num())
        {
            KeyDataB = &Values[Index + 1];

            // 5. Compute the interpolation alpha between the two frames
            float FrameA = Times[Index].Value;
            float FrameB = Times[Index + 1].Value;

            if (FrameB > FrameA)
            {
                // Before the first key the raw alpha is negative: hold the first key's pose
                Alpha = FMath::Clamp( float( ( CurrentTime.AsDecimal() - FrameA ) / ( FrameB - FrameA ) ), 0.0f, 1.0f );
            }
        }
    }

    if ( KeyDataA )
    {
        // Wrap the found structure in the execution token and send it to the engine
        ExecutionTokens.Add( FArianeDrawingExecutionToken( KeyDataA, KeyDataB, Alpha, Operand ) );
    }
};
