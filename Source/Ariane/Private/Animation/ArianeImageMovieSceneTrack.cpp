// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeImageMovieSceneTrack.h"
#include "ArianeImageMovieSceneEvalTemplate.h"
#include "ArianeImage.h"
#include "ArianeLayerDrawing.h"
//Unreal headers
#include "MovieScene.h"
#include "LevelSequence.h"
#include "ISequencer.h"

UArianeImageMovieSceneTrack::UArianeImageMovieSceneTrack()
{
}

bool
UArianeImageMovieSceneTrack::SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const
{
    return SectionClass == UArianeImageMovieSceneSection::StaticClass();
}

UMovieSceneSection*
UArianeImageMovieSceneTrack::CreateNewSection()
{
    UArianeImageMovieSceneSection* NewSection = NewObject<UArianeImageMovieSceneSection>(this, UArianeImageMovieSceneSection::StaticClass(), NAME_None, RF_Transactional);

    return NewSection;
}

UArianeLayerDrawing*
UArianeImageMovieSceneTrack::GetDrawingLayer( ISequencer& InSequencer )
{
    FGuid ObjectGuid = FindObjectBindingGuid();

    // On demande à l'éditeur Sequencer de résoudre l'adresse dans la timeline actuelle
    FMovieSceneSequenceID SequenceID = InSequencer.GetFocusedTemplateID();
    TArrayView<TWeakObjectPtr<>> BoundObjects = InSequencer.FindBoundObjects(ObjectGuid, SequenceID);

    if (BoundObjects.Num() > 0 && BoundObjects[0].IsValid())
    {
        return Cast<UArianeLayerDrawing>(BoundObjects[0].Get());
    }
    return nullptr;
}


FMovieSceneEvalTemplatePtr
UArianeImageMovieSceneTrack::CreateTemplateForSection(const UMovieSceneSection& InSection) const
{
    return FArianeImageMovieSceneEvalTemplate(*CastChecked<UArianeImageMovieSceneSection>(&InSection));
}
