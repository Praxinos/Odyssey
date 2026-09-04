// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

#include "CoreMinimal.h"
#include "ISequencerTrackEditor.h"
#include "ArianeImageMovieSceneTrack.h"
#include "MovieSceneTrackEditor.h"
#include "ISequencer.h"

class FArianeEditorImageMovieSceneTrack : public FMovieSceneTrackEditor
{
public:
    FArianeEditorImageMovieSceneTrack(TSharedRef<ISequencer> InSequencer);
    static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> InSequencer);
    virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> TrackClass) const override;
    virtual void BuildObjectBindingTrackMenu(FMenuBuilder& MenuBuilder, const TArray<FGuid>& ObjectBindings, const UClass* ObjectClass) override;

private:
    void AddTrack(TArray<FGuid> ObjectBindings);
};
