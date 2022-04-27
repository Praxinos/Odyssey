// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "EntitySystem/BuiltInComponentTypes.h"
#include "EntitySystem/TrackInstance/MovieSceneTrackInstance.h"
#include "UObject/ObjectMacros.h"
#include "MovieSceneSingleCameraCutTrackInstance.generated.h"

class UMovieSceneSingleCameraCutSection;
namespace UE { namespace MovieScene { struct FCameraCutAnimator; } }

UCLASS()
class UMovieSceneSingleCameraCutTrackInstance : public UMovieSceneTrackInstance
{
    GENERATED_BODY()

private:
    virtual void OnAnimate() override;
    virtual void OnInputAdded(const FMovieSceneTrackInstanceInput& InInput) override;
    virtual void OnInputRemoved(const FMovieSceneTrackInstanceInput& InInput) override;
    virtual void OnEndUpdateInputs() override;
    virtual void OnDestroyed() override;

private:
    struct FCameraCutCache
    {
        TWeakObjectPtr<> LastLockedCamera;
    };

    struct FCameraCutInputInfo
    {
        FMovieSceneTrackInstanceInput Input;
        float GlobalStartTime = 0.f;
    };

    struct FCameraCutUseData
    {
        int32 UseCount = 0;
        bool bValid = false;
        bool bCanBlend = false;
    };

    FCameraCutCache CameraCutCache;
    TMap<IMovieScenePlayer*, FCameraCutUseData> PlayerUseCounts;
    TArray<FCameraCutInputInfo> SortedInputInfos;

    friend struct UE::MovieScene::FCameraCutAnimator;
};
