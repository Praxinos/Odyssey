// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "MovieSceneSequenceID.h"

#include "ExportSettings.generated.h"

//---

USTRUCT( BlueprintType )
struct FExportMarkSettings
{
    GENERATED_BODY()

public:
    /** Make a panel when an animation cut exists. */
    UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Mark")
    bool AnimationCuts { true };

    /** Make a panel when a sequencer mark exists. */
    UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Mark")
    bool Marks { false };

    /** Make a panel always on the first frame of a shot. */
    UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Mark")
    bool FirstFrameOfShot { false };

    ///** Make a panel every N frames (restart at each shot). */
    //UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Mark")
    //bool EveryNFrameForEachShot { false };

    ///** Interval (start of each shot). */
    //UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Mark")
    //int32 IntervalForEachShot;

    ///** Make a panel every N frames (start of root board). */
    //UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Mark")
    //bool EveryNFrameForRootBoard { false };

    ///** Interval (start of root board). */
    //UPROPERTY(config, BlueprintReadWrite, EditAnywhere, Category="Mark")
    //int32 IntervalForRootBoard;
};

//---

class ISequencer;
class UMovieSceneSequence;

float GetMostRelevantCameraAspectRatio( ISequencer* iSequencer, FMovieSceneSequenceIDRef iRootEposSequenceId );
