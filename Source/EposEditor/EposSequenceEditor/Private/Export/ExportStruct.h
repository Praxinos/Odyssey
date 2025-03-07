// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "MovieScene.h"

#include "EposSequenceHelpers.h"

#include "ExportStruct.generated.h"

//---

class ISequencer;

struct FExportPanelSourceMark
{
    FMovieSceneMarkedFrame  mMark;
};

struct FExportPanelSourceAnimationCut
{
    struct FAnimationCutAndBindingId
    {
        FFrameNumber    mCut;
        FGuid           mBindingId;
    };

    TArray<FAnimationCutAndBindingId>  mAnimationCuts;
};

struct FExportPanelSourceDrawing
{
    struct FDrawingAndBindingId
    {
        FDrawing    mDrawing;
        FGuid       mBindingId;
    };

    TArray<FDrawingAndBindingId>    mDrawings;
};

struct FExportPanel
{
    FFrameNumber GlobalFrame;

    UMovieSceneSequence* mSequence;
    FMovieSceneSequenceID mSequenceId;

    TOptional<FExportPanelSourceMark>           mSourceMark;
    TOptional<FExportPanelSourceAnimationCut>   mSourceAnimationCut;
    TOptional<FExportPanelSourceDrawing>        mSourceDrawing;
};

//---

USTRUCT( BlueprintType )
struct FExportStruct
{
    GENERATED_BODY()

public:
    TArray<FExportPanel> Panels;

    TWeakPtr<ISequencer> mSequencer;
};
