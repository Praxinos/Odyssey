// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "EntitySystem/BuiltInComponentTypes.h"
#include "EntitySystem/TrackInstance/MovieSceneTrackInstance.h"
#include "UObject/ObjectMacros.h"

#include "MovieSceneSingleCameraCutTrackInstance.generated.h"

class IMovieScenePlayer;
class UMovieSceneSingleCameraCutSection;

namespace UE::EposMovieScene
{
    class FCameraCutViewportPreviewer;
    struct FCameraCutAnimator;
}
namespace UE::MovieScene
{
    struct FCameraCutPlaybackCapability;
    struct FOnCameraCutUpdatedParams;
    struct FSequenceInstance;
}

namespace UE::EposMovieScene
{
    using namespace UE::MovieScene;

    // Backwards compatibilty wrapper for camera cut playback capability.
    struct FCameraCutPlaybackCapabilityCompatibilityWrapper
    {
        FCameraCutPlaybackCapabilityCompatibilityWrapper(const FSequenceInstance& SequenceInstance);

        bool ShouldUpdateCameraCut();
        void OnCameraCutUpdated(const FOnCameraCutUpdatedParams& Params);
#if WITH_EDITOR
        bool ShouldRestoreEditorViewports();
#endif

        FCameraCutPlaybackCapability* CameraCutCapability;
        IMovieScenePlayer* Player;
    };
}

/**
 * Track instance used to animate camera cuts.
 */
UCLASS()
class EPOSTRACKS_API UMovieSceneSingleCameraCutTrackInstance : public UMovieSceneTrackInstance
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    /**
     * Toggle camera cut lock on cinematic editor viewports while also correctly managing
     * remember/restoring/discarding pre-animated viewport positions based on sequencer
     * settings.
     */
    static void ToggleCameraCutLock(UMovieSceneEntitySystemLinker* Linker, bool bEnableCameraCuts, bool bRestoreViewports);
#endif

private:
    virtual void OnInitialize() override;
    virtual void OnAnimate() override;
    virtual void OnEndUpdateInputs() override;
    virtual void OnDestroyed() override;

private:
    /**
     * Stores information about the last set camera in order to differentiate
     * between new and pre-existing cuts.
     */
    struct FCameraCutCache
    {
        TWeakObjectPtr<> LastLockedCamera;
        FMovieSceneTrackInstanceInput LastInput;
    };

    /**
     * Track instance input qualified with the global start time of its corresponding
     * section, used for sorting inputs and prioritizing more "recent" camera cuts
     * over "older" ones.
     */
    struct FCameraCutInputInfo
    {
        FMovieSceneTrackInstanceInput Input;
        float GlobalStartTime = 0.f;
    };

    FCameraCutCache CameraCutCache;
    TArray<FCameraCutInputInfo> SortedInputInfos;

#if WITH_EDITOR
    TUniquePtr<UE::EposMovieScene::FCameraCutViewportPreviewer> ViewportPreviewer;
#endif

private:

    friend struct UE::EposMovieScene::FCameraCutAnimator;
};
