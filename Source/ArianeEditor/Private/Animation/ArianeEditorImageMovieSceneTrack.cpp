// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor Headers
#include "ArianeEditorImageMovieSceneTrack.h"
// Ariane Headers
#include "ArianeImageMovieSceneTrack.h"
#include "ArianeLayerDrawing.h"

FArianeEditorImageMovieSceneTrack::FArianeEditorImageMovieSceneTrack(TSharedRef<ISequencer> InSequencer)
    : FMovieSceneTrackEditor(InSequencer)
{
}

TSharedRef<ISequencerTrackEditor>
FArianeEditorImageMovieSceneTrack::CreateTrackEditor(TSharedRef<ISequencer> InSequencer)
{
    return MakeShared<FArianeEditorImageMovieSceneTrack>(InSequencer);
}

bool
FArianeEditorImageMovieSceneTrack::SupportsType(TSubclassOf<UMovieSceneTrack> TrackClass) const
{
    return TrackClass == UArianeImageMovieSceneTrack::StaticClass();
}

void
FArianeEditorImageMovieSceneTrack::BuildObjectBindingTrackMenu( FMenuBuilder& MenuBuilder
                                                              , const TArray<FGuid>& ObjectBindings
                                                              , const UClass* ObjectClass)
{
    if ( ObjectClass->IsChildOf( UArianeLayerDrawing::StaticClass() ) )
    {
        MenuBuilder.AddMenuEntry(
            NSLOCTEXT("Ariane", "ariane-editor-add-image-track", "Ariane Image track"),
            NSLOCTEXT("Ariane", "ariane-editor-add-image-track.tooltip", "Add Image spawn track"),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateSP(this, &FArianeEditorImageMovieSceneTrack::AddTrack, ObjectBindings))
        );
    }
}

void
FArianeEditorImageMovieSceneTrack::AddTrack(TArray<FGuid> ObjectBindings)
{
    TSharedPtr<ISequencer> Sequencer = GetSequencer();
    UMovieScene* MovieScene = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();

    FScopedTransaction AddTrackTransaction(NSLOCTEXT("Ariane", "ariane-editor-add-image-track", "Add Ariane Image Track"));

    for ( const FGuid& Binding : ObjectBindings )
    {
        if (!MovieScene->FindTrack(UArianeImageMovieSceneTrack::StaticClass(), Binding))
        {
            MovieScene->Modify();

            UMovieSceneTrack* NewTrack = MovieScene->AddTrack(UArianeImageMovieSceneTrack::StaticClass(), Binding);

            if (NewTrack)
            {
                NewTrack->Modify();
                UMovieSceneSection* NewSection = NewTrack->CreateNewSection();

                if ( NewSection )
                {
                    // 2. LE LIEN CRUCIAL : On donne une dimension temporelle à la section !
                    // "All()" force la section à être infinie et à s'étendre sur toute la timeline grise
                    NewSection->SetRange(TRange<FFrameNumber>::All());

                    // 3. L'ENREGISTREMENT EFFECTIF : On l'ajoute physiquement dans le tableau de la piste
                    NewTrack->AddSection(*NewSection);
                }

                Sequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
            }
        }
    }
}
