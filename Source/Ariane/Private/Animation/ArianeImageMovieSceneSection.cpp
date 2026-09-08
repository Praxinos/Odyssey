// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeImageMovieSceneSection.h"
#include "ArianeImageMovieSceneTrack.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneChannelData.h"
#include "ArianeImage.h"
#include "ArianeGroup.h"
#include "ArianeLayerDrawing.h"
#include "MovieScene.h"
#include "MovieSceneTrack.h"
#include "ISequencer.h"

namespace Sequencer
{
/*
    template<>
    FKeyHandle EvaluateAndAddKey(
        FArianeImageMovieSceneChannel* InChannel,
        const TMovieSceneChannelData<FArianeImageKeyData>& InChannelData,
        FFrameNumber InTime,
        ISequencer& InSequencer,
        FArianeImageKeyData InDefaultValue)
    {
        UArianeLayerDrawing* MyLayer = nullptr;

        using namespace UE::MovieScene;

        TArray< UMovieSceneTrack* > SelectedTracks;
        InSequencer.GetSelectedTracks ( SelectedTracks );

        if( SelectedTracks.Num() )
        {
            // 2. LA TRACK SANS ERREUR :
            // L'Outer d'une section est TOUJOURS sa Track
            UArianeImageMovieSceneTrack* MyTrack = Cast<UArianeImageMovieSceneTrack>(SelectedTracks[0]);

            if (MyTrack)
            {
                // 3. LE LAYER VIA LA TRACK :
                // On récupère le film (MovieScene) qui contient la piste
                UMovieScene* MovieScene = MyTrack->GetTypedOuter<UMovieScene>();
                if (MovieScene)
                {
                    FGuid ObjectBindingID;
                    if (MovieScene->FindTrackBinding(*MyTrack, ObjectBindingID))
                    {
                        for (TWeakObjectPtr<> BoundObject : InSequencer.FindObjectsInCurrentSequence (ObjectBindingID))
                        {
                            if ( UArianeLayerDrawing* FoundLayer = Cast<UArianeLayerDrawing>( BoundObject.Get() ) )
                            {
                                MyLayer = FoundLayer;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // ... Votre logique de AddKey existante ...
        //return FKeyHandle::Invalid();




        FArianeImageKeyData ValueAtTime = InDefaultValue;
        EvaluateChannel(InChannel, InTime, ValueAtTime);

        if( MyLayer )
        {
            ValueAtTime.Image = NewObject<UArianeImage>(MyLayer, UArianeImage::StaticClass(), NAME_None, RF_Transactional);
        }

        EMovieSceneKeyInterpolation InterpolationMode = GetInterpolationMode(InChannel, InTime, InSequencer.GetKeyInterpolation());

        return AddKeyToChannel(InChannel, InTime, ValueAtTime, InterpolationMode);
    }
*/
    template<>
    FKeyHandle AddOrUpdateKey(
        FArianeImageMovieSceneChannel* InChannel,
        UMovieSceneSection* InSectionToKey,
        FFrameNumber InTime,
        ISequencer& InSequencer,
        const FGuid& InObjectBindingID,
        FTrackInstancePropertyBindings* InPropertyBindings)
    {
        UArianeImageMovieSceneTrack* Track = Cast<UArianeImageMovieSceneTrack>(InSectionToKey->GetOuter());
        UArianeLayerDrawing* DrawingLayer = Track->GetDrawingLayer( InSequencer );

        // 1. Accès direct aux tableaux de clés du Channel
        TMovieSceneChannelData<FArianeImageKeyData> ChannelData = InChannel->GetData();

        // 2. Recherche : est-ce qu'une clé existe déjà à cette frame exacte ?
        int32 ExistingIndex = Algo::BinarySearch(ChannelData.GetTimes(), InTime);

        FArianeImageKeyData NewKeyData;

        if (ExistingIndex != INDEX_NONE)
        {
            // CAS A : L'utilisateur clique sur un losange EXISTANT -> On fait une mise à jour
            NewKeyData = ChannelData.GetValues()[ExistingIndex];
            if (!NewKeyData.Image)
            {
                NewKeyData.Image = NewObject<UArianeImage>(InSectionToKey, UArianeImage::StaticClass(), NAME_None, RF_Transactional);

                NewKeyData.Image->GetRootGroup()->UpdateTransform();
                DrawingLayer->Update( false );
            }
            ChannelData.GetValues()[ExistingIndex] = NewKeyData;

            return ChannelData.GetHandle(ExistingIndex);
        }
        else
        {
            // CAS B : L'utilisateur clique dans le vide -> VRAIE CRÉATION d'une clé unique !
            // On cherche le losange précédent pour cloner son contenu (pour ne pas perdre les tracés précédents)
            const int32 PrevIndex = Algo::UpperBound(ChannelData.GetTimes(), InTime) - 1;

            //if (PrevIndex >= 0 && ChannelData.GetValues()[PrevIndex].Image != nullptr)
            //{
                // IMPORTANT : On fait un DuplicateObject pour allouer une NOUVELLE instance unique en RAM !
            //    NewKeyData.Image = DuplicateObject<UArianeImage>(ChannelData.GetValues()[PrevIndex].Image, DrawingLayer);
            //}
            //else
            {
                // Si pas de clé avant, on crée une image vierge de zéro
                NewKeyData.Image = NewObject<UArianeImage>(InSectionToKey, UArianeImage::StaticClass(), NAME_None, RF_Transactional);

                NewKeyData.Image->GetRootGroup()->UpdateTransform();
                DrawingLayer->Update( false );
            }

            // On injecte le temps et la donnée unique directement dans le tableau d'Unreal
            int32 NewIndex = ChannelData.AddKey(InTime, NewKeyData);

            return ChannelData.GetHandle(NewIndex);
        }
    }
}

bool FArianeImageMovieSceneChannel::Evaluate(FFrameTime InTime, FArianeImageKeyData& OutValue) const
{
    if (Times.Num() == 0) return false;

    const int32 Index = Algo::UpperBound(Times, InTime.FrameNumber) - 1;
    if (Index < 0) return false;

    // Lecture simple de l'adresse existante, ultra-rapide pour le flip
    OutValue = Values[Index];
    return true;
}

void FArianeImageMovieSceneChannel::GetKeys(const TRange<FFrameNumber>& WithinRange, TArray<FFrameNumber>* OutKeyTimes, TArray<FKeyHandle>* OutKeyHandles)
{
    GetData().GetKeys(WithinRange, OutKeyTimes, OutKeyHandles);
}

void FArianeImageMovieSceneChannel::GetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<FFrameNumber> OutKeyTimes)
{
    GetData().GetKeyTimes(InHandles, OutKeyTimes);
}

void FArianeImageMovieSceneChannel::SetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<const FFrameNumber> InKeyTimes)
{
    GetData().SetKeyTimes(InHandles, InKeyTimes);
}

void FArianeImageMovieSceneChannel::DuplicateKeys(TArrayView<const FKeyHandle> InHandles, TArrayView<FKeyHandle> OutNewHandles)
{
    GetData().DuplicateKeys(InHandles, OutNewHandles);
}

void FArianeImageMovieSceneChannel::DeleteKeys(TArrayView<const FKeyHandle> InHandles)
{
    GetData().DeleteKeys(InHandles);
}

void FArianeImageMovieSceneChannel::DeleteKeysFrom(FFrameNumber InTime, bool bDeleteKeysBefore)
{
    GetData().DeleteKeysFrom(InTime, bDeleteKeysBefore);
}

void FArianeImageMovieSceneChannel::ChangeFrameResolution(FFrameRate SourceRate, FFrameRate DestinationRate)
{
    GetData().ChangeFrameResolution(SourceRate, DestinationRate);
}

TRange<FFrameNumber> FArianeImageMovieSceneChannel::ComputeEffectiveRange() const
{
    return GetData().GetTotalRange();
}

int32 FArianeImageMovieSceneChannel::GetNumKeys() const
{
    return Times.Num();
}

void FArianeImageMovieSceneChannel::Reset()
{
    Times.Reset();
    Values.Reset();
    KeyHandles.Reset();
}

void FArianeImageMovieSceneChannel::Offset(FFrameNumber DeltaPosition)
{
    GetData().Offset(DeltaPosition);
}

UArianeImageMovieSceneSection::UArianeImageMovieSceneSection()
{
#if WITH_EDITOR
    // 1. On prépare les métadonnées pour l'affichage du losange dans le Sequencer
    FMovieSceneChannelMetaData ChannelMetaData;
    ChannelMetaData.Name = TEXT("ArianeImageSpawnTrack");
    ChannelMetaData.DisplayText = NSLOCTEXT("Ariane", "ArianeImageSpawnTrack_Text", "Image Key");
#endif

    // Force la section à restaurer l'état d'origine à la fin de la lecture
    // ou lors de la fermeture du Séquenceur.
SetCompletionMode(EMovieSceneCompletionMode::RestoreState);

    // 2. Le mécanisme officiel d'Unreal : On crée le conteneur de préparation
    FMovieSceneChannelProxyData ProxyData;

    // 3. LA VRAIE FONCTION : On enregistre le canal dans le builder
#if WITH_EDITOR
    ProxyData.Add(ImageChannel, ChannelMetaData);
#else
    ProxyData.AddChannel(&ImageEventChannel);
#endif

    // 4. On écrase le proxy par défaut de la section en lui injectant notre structure
    // (Cette ligne compile et initialise proprement le système de clés du Sequencer)
    ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(ProxyData));
}
