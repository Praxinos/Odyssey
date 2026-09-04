// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeImageMovieSceneEvalTemplate.h"
#include "ArianeImage.h"
#include "ArianeLayerDrawing.h"

FArianeImageExecutionToken::FArianeImageExecutionToken( const FArianeImageKeyData& InKeyData, const FMovieSceneEvaluationOperand& InOperand )
    : KeyData(InKeyData), StoredOperand( InOperand )
{
}

void FArianeImageExecutionToken::Execute( const FMovieSceneContext& Context
                                        , const FMovieSceneEvaluationOperand& Operand
                                        , FPersistentEvaluationData& PersistentData
                                        , IMovieScenePlayer& Player )
{
    // 1. Résoudre l'objet lié à la piste
    TArrayView<TWeakObjectPtr<UObject>> BoundObjects = Player.FindBoundObjects( StoredOperand.ObjectBindingID, StoredOperand.SequenceID );

    if ( BoundObjects.Num() > 0 )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>( BoundObjects[0].Get() );

        if ( DrawingLayer && ( DrawingLayer->GetImage() != KeyData.Image ) )
        {
            DrawingLayer->SetImage(KeyData.Image);
        }
    }
}

FArianeImageMovieSceneEvalTemplate::FArianeImageMovieSceneEvalTemplate()
{
}

FArianeImageMovieSceneEvalTemplate::FArianeImageMovieSceneEvalTemplate( const UArianeImageMovieSceneSection& Section )
    : FMovieSceneEvalTemplate()
{
}

UScriptStruct& FArianeImageMovieSceneEvalTemplate::GetScriptStructImpl() const
{
    return *StaticStruct();
}

void
FArianeImageMovieSceneEvalTemplate::Evaluate( const FMovieSceneEvaluationOperand& Operand
                                            , const FMovieSceneContext& Context
                                            , const FPersistentEvaluationData& PersistentData
                                            , FMovieSceneExecutionTokens& ExecutionTokens ) const
{
    // On récupère la section source liée à ce template
    const UArianeImageMovieSceneSection* ArianeSection = Cast<UArianeImageMovieSceneSection>(GetSourceSection());
    if (!ArianeSection) return;

    FArianeImageKeyData ActiveKeyData;

    // =========================================================================
    // LE MAILLON MANQUANT : On force l'appel au Evaluate du Canal !
    // C'est cette ligne qui va enfin allumer votre breakpoint dans le Channel.
    // =========================================================================
    if (ArianeSection->ImageChannel.Evaluate(Context.GetTime(), ActiveKeyData))
    {
        // On encapsule la structure trouvée dans le jeton d'exécution et on l'envoie au moteur
        ExecutionTokens.Add( FArianeImageExecutionToken( ActiveKeyData, Operand ) );
    }
};
