// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeDrawingMovieSceneEvalTemplate.h"
#include "ArianeDrawing.h"
#include "ArianeGroup.h"
#include "ArianeLayerVector.h"
#include "Evaluation/MovieSceneAnimTypeID.h"

#include "Evaluation/MovieScenePreAnimatedState.h"

// 1. Le Token : Il stocke l'état d'origine et sait comment le restaurer
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
            // On restaure l'ancienne image sauvegardée avant l'animation
            VectorLayer->SetDrawing(OldDrawing.Get());

            VectorLayer->GetDrawing()->GetRootGroup()->UpdateTransform();
            VectorLayer->Update( false );
        }
    }
};

// 2. Le Producer : Il est appelé par Unreal pour fabriquer le Token ci-dessus
struct FArianeDrawingPreAnimatedTokenProducer : IMovieScenePreAnimatedTokenProducer
{
    virtual IMovieScenePreAnimatedTokenPtr CacheExistingState(UObject& Object) const override
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(&Object);
        if (VectorLayer)
        {
            // On capture l'image actuelle (l'image de base de la map)
            return FArianeDrawingPreAnimatedToken(VectorLayer->GetDrawing());
        }
        return IMovieScenePreAnimatedTokenPtr();
    }
};


// Déclarer l'identifiant unique pour la restauration de l'ArianeDrawing
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
    // 1. Résoudre l'objet lié à la piste
    TArrayView<TWeakObjectPtr<UObject>> BoundObjects = Player.FindBoundObjects( StoredOperand.ObjectBindingID, StoredOperand.SequenceID );

    if ( BoundObjects.Num() > 0 )
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>( BoundObjects[0].Get() );
        EMovieScenePlayerStatus::Type PlaybackStatus = Player.GetPlaybackStatus();

        // LA CORRECTION : Enregistrer l'état actuel (l'image par défaut) avant de la modifier.
        // On utilise le 'PreAnimatedState' du Player.
        Player.SavePreAnimatedState( *VectorLayer
                                   , GetArianeDrawingAnimTypeID()
                                   , FArianeDrawingPreAnimatedTokenProducer () );

        if ( KeyData->Drawing && VectorLayer )
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
    // On récupère la section source liée à ce template
    const UArianeDrawingMovieSceneSection* ArianeSection = Cast<UArianeDrawingMovieSceneSection>(GetSourceSection());
    if (!ArianeSection) return;

    // 1. Obtenir le temps actuel du Sequencer
    const FFrameTime CurrentTime = Context.GetTime();

    // 2. Récupérer l'accès aux données de votre canal
    // Note : On assume ici que vous avez stocké votre canal 'MyChannel' dans le template lors de sa compilation
    TMovieSceneChannelData<const FArianeDrawingKeyData> ChannelData = Section->DrawingChannel.GetData();

    // Le tableau des temps géré par votre canal
    TArrayView<const FFrameNumber> Times = ChannelData.GetTimes();
    // Le tableau de vos structures de données
    TArrayView<const FArianeDrawingKeyData> Values = ChannelData.GetValues();

    const FArianeDrawingKeyData* KeyDataA = nullptr;
    const FArianeDrawingKeyData* KeyDataB = nullptr;
    float Alpha = 0.0f;

    if (Times.Num() > 0)
    {
        // 3. Recherche binaire sur le tableau natif des FFrameNumber
        int32 Index = Algo::UpperBound(Times, CurrentTime.GetFrame()) - 1;
        Index = FMath::Clamp(Index, 0, Times.Num() - 1);

        // Clé Actuelle (Borne inférieure)
        KeyDataA = &Values[Index];

        // 4. Clé Suivante (Borne supérieure) s'il y en a une, pour calculer le Lerp
        if (Index + 1 < Times.Num())
        {
            KeyDataB = &Values[Index + 1];

            // 5. Calcul de l'Alpha d'interpolation entre les deux frames
            float FrameA = Times[Index].Value;
            float FrameB = Times[Index + 1].Value;

            if (FrameB > FrameA)
            {
                Alpha = (CurrentTime.AsDecimal() - FrameA) / (FrameB - FrameA);
            }
        }
    }

    if ( KeyDataA )
    {
        // On encapsule la structure trouvée dans le jeton d'exécution et on l'envoie au moteur
        ExecutionTokens.Add( FArianeDrawingExecutionToken( KeyDataA, KeyDataB, Alpha, Operand ) );
    }
};
