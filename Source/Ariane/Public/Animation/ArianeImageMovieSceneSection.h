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
#include "ArianeImageKeyData.h"
#include "ArianeImageMovieSceneChannel.h"
#include "ArianeImage.h"

#include "ArianeImageMovieSceneSection.generated.h"

class UArianeImage;

UCLASS()
class ARIANE_API UArianeImageMovieSceneSection : public UMovieSceneSection
{
    GENERATED_BODY()

public:
    UArianeImageMovieSceneSection();

    virtual void PostLoad() override;
    virtual void PostEditUndo() override;

    UPROPERTY()
    FArianeImageMovieSceneChannel ImageChannel;
};


#if WITH_EDITOR
namespace Sequencer
{
    template<>
    ARIANE_API FKeyHandle AddOrUpdateKey(
        FArianeImageMovieSceneChannel* InChannel,
        UMovieSceneSection* InSectionToKey,
        FFrameNumber InTime,
        ISequencer& InSequencer,
        const FGuid& InObjectBindingID,
        FTrackInstancePropertyBindings* InPropertyBindings);
}
#endif
