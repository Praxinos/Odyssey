// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeDrawingMovieSceneSection.h"
#include "ArianeDrawingMovieSceneTrack.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneChannelData.h"
#include "ArianeDrawing.h"
#include "ArianeGroup.h"
#include "ArianeLayerVector.h"
#include "MovieScene.h"
#include "MovieSceneTrack.h"
#if WITH_EDITOR
#include "ISequencer.h"

namespace Sequencer
{
    template<>
    FKeyHandle AddOrUpdateKey(
        FArianeDrawingMovieSceneChannel* InChannel,
        UMovieSceneSection* InSectionToKey,
        FFrameNumber InTime,
        ISequencer& InSequencer,
        const FGuid& InObjectBindingID,
        FTrackInstancePropertyBindings* InPropertyBindings)
    {
        UArianeDrawingMovieSceneTrack* Track = Cast<UArianeDrawingMovieSceneTrack>(InSectionToKey->GetOuter());
        UArianeLayerVector* VectorLayer = Track->GetVectorLayer( InSequencer );

        // 1. Direct access to the channel's key arrays
        TMovieSceneChannelData<FArianeDrawingKeyData> ChannelData = InChannel->GetData();

        // 2. Search: does a key already exist at this exact frame?
        int32 ExistingIndex = Algo::BinarySearch(ChannelData.GetTimes(), InTime);

        // If the key already exists, we update it completely
        if ( ExistingIndex != INDEX_NONE )
        {
            FArianeDrawingKeyData* CurrentKeyData = nullptr;

            CurrentKeyData = &ChannelData.GetValues()[ExistingIndex];

            // note: VectorLayer->GetDrawing() holds the current key's image
            CurrentKeyData->Drawing = DuplicateObject(VectorLayer->GetDrawing(), InSectionToKey);
            CurrentKeyData->RecordGeometry( CurrentKeyData->Drawing );

            CurrentKeyData->Drawing->GetRootGroup()->UpdateTransform();
            // Note: will also update GUI via delegates if any is registered
            VectorLayer->Update( false );

            return ChannelData.GetHandle(ExistingIndex);
        }
        else // or else create a new key
        {
            int32 NewKeyIndex = ChannelData.AddKey( InTime, FArianeDrawingKeyData() );
            // we work on the pointer in order not to copy member variable, especially arrays of instanced struct
            FArianeDrawingKeyData* CurrentKeyData = &ChannelData.GetValues()[NewKeyIndex];

            // By duplicating the image, we also retrieve the same IDs for objects, vertices, segments etc...
            // this will allow any interpolated object to find its counter part in the other key by searching by Guid
            CurrentKeyData->Drawing = DuplicateObject( VectorLayer->GetDrawing(), InSectionToKey);

            CurrentKeyData->RecordGeometry( CurrentKeyData->Drawing );

            CurrentKeyData->Drawing->GetRootGroup()->UpdateTransform();
            // Note: will also update GUI via delegates if any is registered
            VectorLayer->Update( false );

            return ChannelData.GetHandle(NewKeyIndex);
        }
    }
}
#endif // WITH_EDITOR

UArianeDrawingMovieSceneSection::UArianeDrawingMovieSceneSection()
{
#if WITH_EDITOR
    // 1. Prepare the metadata used to display the key diamond in the Sequencer
    FMovieSceneChannelMetaData ChannelMetaData;
    ChannelMetaData.Name = TEXT("ArianeDrawingSpawnTrack");
    ChannelMetaData.DisplayText = NSLOCTEXT("Ariane", "ArianeDrawingSpawnTrack_Text", "Drawing Key");
#endif

    // Forces the section to restore the original state at the end of playback
    // or when the Sequencer is closed.
SetCompletionMode(EMovieSceneCompletionMode::RestoreState);

    // 2. Unreal's official mechanism: create the preparation container
    FMovieSceneChannelProxyData ProxyData;

    // 3. THE ACTUAL CALL: register the channel in the builder
#if WITH_EDITOR
    ProxyData.Add(DrawingChannel, ChannelMetaData);
#else
    ProxyData.AddChannel(&DrawingEventChannel);
#endif

    // 4. Override the section's default proxy by injecting our structure
    // (This line compiles and properly initializes the Sequencer's key system)
    ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(ProxyData));
}

void
UArianeDrawingMovieSceneSection::PostLoad()
{
    Super::PostLoad();

    DrawingChannel.PostLoad();
}

#if WITH_EDITOR
void
UArianeDrawingMovieSceneSection::PostEditUndo()
{
    Super::PostEditUndo();

    DrawingChannel.PostEditUndo();
}
#endif
