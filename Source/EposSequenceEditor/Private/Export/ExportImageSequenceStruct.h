// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "MovieScene.h"

#include "EposSequenceHelpers.h"

#include "ExportImageSequenceStruct.generated.h"

//---

struct FExportImageSequencePanelSourceMark
{
    FMovieSceneMarkedFrame  mMark;
};

struct FExportImageSequencePanelSourceDrawing
{
    struct FDrawingAndBindingId
    {
        FDrawing    mDrawing;
        FGuid       mBindingId;
    };

    TArray<FDrawingAndBindingId>    mDrawings;
};

USTRUCT()
struct FExportImageSequencePanel
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ExportImageSequenceStruct )
    FFrameNumber GlobalFrame;

    UMovieSceneSequence* mSequence;

    TOptional<FExportImageSequencePanelSourceMark>      mSourceMark;
    TOptional<FExportImageSequencePanelSourceDrawing>   mSourceDrawing;
};

//---

USTRUCT()
struct FExportImageSequenceStruct
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ExportImageSequenceStruct )
    TArray<FExportImageSequencePanel> Panels;
};
