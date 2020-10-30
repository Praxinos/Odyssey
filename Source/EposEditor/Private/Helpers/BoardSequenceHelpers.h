// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"

class UMovieScene;
class UMovieSceneTrack;
class ISequencer;

class EPOSEDITOR_API BoardSequenceHelpers
{
public:
    /**
    *  Find a Camera from the camera track
    *
    * @param TSharedPtr<ISequencer> iSequencer to add Camera track and CameraCut track.
    */
    static void ArrangeSections( ISequencer* iSequencer );
};
