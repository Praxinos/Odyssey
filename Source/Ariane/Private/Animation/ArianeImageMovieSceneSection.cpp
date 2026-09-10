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

        if ( ExistingIndex != INDEX_NONE )
        {
            FArianeImageKeyData* CurrentKeyData = nullptr;

            // CAS A : L'utilisateur clique sur un losange EXISTANT -> On fait une mise à jour
            CurrentKeyData = &ChannelData.GetValues()[ExistingIndex];

            // note: DrawingLayer->GetImage() holds the current key's image
            CurrentKeyData->Image = DuplicateObject(DrawingLayer->GetImage(), InSectionToKey);
            CurrentKeyData->RecordGeometry( CurrentKeyData->Image );

            //CurrentKeyData->Image->GetRootGroup()->UpdateTransform();
            //DrawingLayer->Update( false );

            return ChannelData.GetHandle(ExistingIndex);
        }
        else
        {
            // CAS B : L'utilisateur clique dans le vide -> VRAIE CRÉATION d'une clé unique !
            // On cherche le losange précédent pour cloner son contenu (pour ne pas perdre les tracés précédents)
            const int32 PrevIndex = Algo::UpperBound(ChannelData.GetTimes(), InTime) - 1;
            // On injecte le temps et la donnée unique directement dans le tableau d'Unreal
            int32 NewKeyIndex = ChannelData.AddKey(InTime, FArianeImageKeyData());
            FArianeImageKeyData* CurrentKeyData = &ChannelData.GetValues()[NewKeyIndex];

            CurrentKeyData->Image = DuplicateObject(DrawingLayer->GetImage(), InSectionToKey);

            CurrentKeyData->RecordGeometry( CurrentKeyData->Image );

            //CurrentKeyData->Image->GetRootGroup()->UpdateTransform();
            //DrawingLayer->Update( false );

            return ChannelData.GetHandle(NewKeyIndex);
        }
    }
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

void
UArianeImageMovieSceneSection::PostLoad()
{
    Super::PostLoad();

    ImageChannel.PostLoad();
}

void
UArianeImageMovieSceneSection::PostEditUndo()
{
    Super::PostEditUndo();

    ImageChannel.PostEditUndo();
}
