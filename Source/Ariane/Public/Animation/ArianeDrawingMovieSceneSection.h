// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal Headers
#include "CoreMinimal.h"
#include "MovieSceneSection.h"
#include "Channels/MovieSceneEventChannel.h"
#include "SequencerChannelTraits.h"
#include "ScopedTransaction.h"
#include "Channels/MovieSceneChannelData.h"
// Ariane Headers
#include "ArianeDrawingKeyData.h"
#include "ArianeDrawingMovieSceneChannel.h"
#include "ArianeDrawing.h"

#include "ArianeDrawingMovieSceneSection.generated.h"

class UArianeDrawing;

UCLASS()
class ARIANE_API UArianeDrawingMovieSceneSection : public UMovieSceneSection
{
    GENERATED_BODY()

public:
    UArianeDrawingMovieSceneSection();

    virtual void PostLoad() override;
    virtual void PostEditUndo() override;

    UPROPERTY()
    FArianeDrawingMovieSceneChannel DrawingChannel;
};


#if WITH_EDITOR
namespace Sequencer
{
    template<>
    ARIANE_API FKeyHandle AddOrUpdateKey(
        FArianeDrawingMovieSceneChannel* InChannel,
        UMovieSceneSection* InSectionToKey,
        FFrameNumber InTime,
        ISequencer& InSequencer,
        const FGuid& InObjectBindingID,
        FTrackInstancePropertyBindings* InPropertyBindings);
}
#endif
