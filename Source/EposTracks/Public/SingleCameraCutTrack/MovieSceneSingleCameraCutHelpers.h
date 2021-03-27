// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class UMovieSceneSequence;

class EPOSTRACKS_API MovieSceneSingleCameraCutHelpers
{
public:
    static TArray<FFrameTime> GetCameraTransformKeys( UMovieSceneSequence* iShotSequence );
};
