// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor Headers
#include "ArianeEditorDrawingMovieSceneTrack.h"
// Ariane Headers
#include "ArianeDrawingMovieSceneTrack.h"
#include "ArianeLayerVector.h"

FArianeEditorDrawingMovieSceneTrack::FArianeEditorDrawingMovieSceneTrack(TSharedRef<ISequencer> InSequencer)
    : FMovieSceneTrackEditor(InSequencer)
{
}

TSharedRef<ISequencerTrackEditor>
FArianeEditorDrawingMovieSceneTrack::CreateTrackEditor(TSharedRef<ISequencer> InSequencer)
{
    return MakeShared<FArianeEditorDrawingMovieSceneTrack>(InSequencer);
}

bool
FArianeEditorDrawingMovieSceneTrack::SupportsType(TSubclassOf<UMovieSceneTrack> TrackClass) const
{
    return TrackClass == UArianeDrawingMovieSceneTrack::StaticClass();
}

void
FArianeEditorDrawingMovieSceneTrack::BuildObjectBindingTrackMenu( FMenuBuilder& MenuBuilder
                                                                , const TArray<FGuid>& ObjectBindings
                                                                , const UClass* ObjectClass)
{
    if ( ObjectClass->IsChildOf( UArianeLayerVector::StaticClass() ) )
    {
        MenuBuilder.AddMenuEntry(
            NSLOCTEXT("Ariane", "ariane-editor-add-image-track", "Ariane Drawing track"),
            NSLOCTEXT("Ariane", "ariane-editor-add-image-track.tooltip", "Add Drawing spawn track"),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateSP(this, &FArianeEditorDrawingMovieSceneTrack::AddTrack, ObjectBindings))
        );
    }
}

void
FArianeEditorDrawingMovieSceneTrack::AddTrack(TArray<FGuid> ObjectBindings)
{
    TSharedPtr<ISequencer> Sequencer = GetSequencer();
    UMovieScene* MovieScene = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();

    FScopedTransaction AddTrackTransaction(NSLOCTEXT( "Ariane"
                                                    , "ariane-editor-add-drawing-track"
                                                    , "Add Ariane Drawing Track"));

    for ( const FGuid& Binding : ObjectBindings )
    {
        if (!MovieScene->FindTrack(UArianeDrawingMovieSceneTrack::StaticClass(), Binding))
        {
            MovieScene->Modify();

            UArianeDrawingMovieSceneTrack* NewTrack = Cast<UArianeDrawingMovieSceneTrack>( MovieScene->AddTrack(UArianeDrawingMovieSceneTrack::StaticClass(), Binding ) );

            if ( NewTrack )
            {
                UMovieSceneSection* NewSection = NewTrack->CreateNewSection();
                static const FName ImagePropertyName("Drawing");

                NewTrack->SetPropertyNameAndPath( ImagePropertyName, ImagePropertyName.ToString() );

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
