// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeImageMovieSceneEvalTemplate.h"
#include "ArianeImage.h"
#include "ArianeGroup.h"
#include "ArianeLayerDrawing.h"
#include "Evaluation/MovieSceneAnimTypeID.h"

#include "Evaluation/MovieScenePreAnimatedState.h"

// 1. Le Token : Il stocke l'état d'origine et sait comment le restaurer
struct FArianeImagePreAnimatedToken : IMovieScenePreAnimatedToken
{
    TWeakObjectPtr<UArianeImage> OldImage;

    FArianeImagePreAnimatedToken(UArianeImage* InImage) : OldImage(InImage) {}

    virtual void RestoreState(UObject& Object, const UE::MovieScene::FRestoreStateParams& Params) override
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(&Object);
        if (DrawingLayer)
        {
            // On restaure l'ancienne image sauvegardée avant l'animation
            DrawingLayer->SetImage(OldImage.Get());

            DrawingLayer->GetImage()->GetRootGroup()->UpdateTransform();
            DrawingLayer->Update( false );
        }
    }
};

// 2. Le Producer : Il est appelé par Unreal pour fabriquer le Token ci-dessus
struct FArianeImagePreAnimatedTokenProducer : IMovieScenePreAnimatedTokenProducer
{
    virtual IMovieScenePreAnimatedTokenPtr CacheExistingState(UObject& Object) const override
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(&Object);
        if (DrawingLayer)
        {
            // On capture l'image actuelle (l'image de base de la map)
            return FArianeImagePreAnimatedToken(DrawingLayer->GetImage());
        }
        return IMovieScenePreAnimatedTokenPtr();
    }
};


// Déclarer l'identifiant unique pour la restauration de l'ArianeImage
static
FMovieSceneAnimTypeID GetArianeImageAnimTypeID()
{
    static FMovieSceneAnimTypeID TypeID = FMovieSceneAnimTypeID::Unique();

    return TypeID;
}

FArianeImageExecutionToken::FArianeImageExecutionToken( const FArianeImageKeyData* InKeyData
                                                      , const FArianeImageKeyData* InNextKeyData
                                                      , float InT
                                                      , const FMovieSceneEvaluationOperand& InOperand )
    : KeyData(InKeyData)
    , NextKeyData(InNextKeyData)
    , T ( InT )
    , StoredOperand( InOperand )
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
        EMovieScenePlayerStatus::Type PlaybackStatus = Player.GetPlaybackStatus();

        // LA CORRECTION : Enregistrer l'état actuel (l'image par défaut) avant de la modifier.
        // On utilise le 'PreAnimatedState' du Player.
        Player.SavePreAnimatedState( *DrawingLayer
                                   , GetArianeImageAnimTypeID()
                                   , FArianeImagePreAnimatedTokenProducer () );

        if ( DrawingLayer )
        {
            bool bInteractive =( ( PlaybackStatus == EMovieScenePlayerStatus::Type::Scrubbing )
                              || ( PlaybackStatus == EMovieScenePlayerStatus::Type::Playing   ) ) ? true : false;

            DrawingLayer->SetImage( KeyData->Image, bInteractive ? false : true );

            DrawingLayer->GetImage()->Animate( KeyData, NextKeyData, T );

            DrawingLayer->GetImage()->GetRootGroup()->UpdateTransform();
            DrawingLayer->Update( bInteractive );
        }
    }
}

FArianeImageMovieSceneEvalTemplate::FArianeImageMovieSceneEvalTemplate()
{
}

FArianeImageMovieSceneEvalTemplate::FArianeImageMovieSceneEvalTemplate( const UArianeImageMovieSceneSection* InSection )
    : FMovieSceneEvalTemplate()
    , Section( InSection )
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

    // 1. Obtenir le temps actuel du Sequencer
    const FFrameTime CurrentTime = Context.GetTime();

    // 2. Récupérer l'accès aux données de votre canal
    // Note : On assume ici que vous avez stocké votre canal 'MyChannel' dans le template lors de sa compilation
    TMovieSceneChannelData<const FArianeImageKeyData> ChannelData = Section->ImageChannel.GetData();

    // Le tableau des temps géré par votre canal
    TArrayView<const FFrameNumber> Times = ChannelData.GetTimes();
    // Le tableau de vos structures de données
    TArrayView<const FArianeImageKeyData> Values = ChannelData.GetValues();

    const FArianeImageKeyData* KeyDataA = nullptr;
    const FArianeImageKeyData* KeyDataB = nullptr;
    float Alpha = 0.0f;

    if (Times.Num() > 0)
    {
        // 3. Recherche binaire sur le tableau natif des FFrameNumber
        int32 Index = Algo::LowerBound(Times, CurrentTime.GetFrame()) - 1;
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
        ExecutionTokens.Add( FArianeImageExecutionToken( KeyDataA, KeyDataB, Alpha, Operand ) );
    }
};
