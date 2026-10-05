// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeDrawingMovieSceneTrack.h"
#include "ArianeDrawingMovieSceneEvalTemplate.h"
#include "ArianeDrawing.h"
#include "ArianeLayerVector.h"
//Unreal headers
#include "MovieScene.h"
#include "LevelSequence.h"
#include "ISequencer.h"

UArianeDrawingMovieSceneTrack::UArianeDrawingMovieSceneTrack()
{
}

bool
UArianeDrawingMovieSceneTrack::SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const
{
    return SectionClass == UArianeDrawingMovieSceneSection::StaticClass();
}

UMovieSceneSection*
UArianeDrawingMovieSceneTrack::CreateNewSection()
{
    UArianeDrawingMovieSceneSection* NewSection = NewObject<UArianeDrawingMovieSceneSection>(this, UArianeDrawingMovieSceneSection::StaticClass(), NAME_None, RF_Transactional);

    return NewSection;
}

UArianeLayerVector*
UArianeDrawingMovieSceneTrack::GetVectorLayer( ISequencer& InSequencer )
{
    FGuid ObjectGuid = FindObjectBindingGuid();
    FMovieSceneSequenceID SequenceID = InSequencer.GetFocusedTemplateID();
    TArrayView<TWeakObjectPtr<>> BoundObjects = InSequencer.FindBoundObjects(ObjectGuid, SequenceID);

    if (BoundObjects.Num() > 0 && BoundObjects[0].IsValid())
    {
        return Cast<UArianeLayerVector>(BoundObjects[0].Get());
    }
    return nullptr;
}


FMovieSceneEvalTemplatePtr
UArianeDrawingMovieSceneTrack::CreateTemplateForSection(const UMovieSceneSection& InSection) const
{
    return FArianeDrawingMovieSceneEvalTemplate(CastChecked<UArianeDrawingMovieSceneSection>(&InSection));
}
