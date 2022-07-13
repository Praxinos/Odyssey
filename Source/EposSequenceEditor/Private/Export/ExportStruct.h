// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "MovieScene.h"

#include "EposSequenceHelpers.h"

#include "ExportStruct.generated.h"

//---

struct FExportPanelSourceMark
{
    FMovieSceneMarkedFrame  mMark;
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

USTRUCT()
struct FExportPanel
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ExportStruct )
    FFrameNumber GlobalFrame;

    UMovieSceneSequence* mSequence;

    TOptional<FExportPanelSourceMark>      mSourceMark;
    TOptional<FExportPanelSourceDrawing>   mSourceDrawing;
};

//---

USTRUCT( BlueprintType )
struct FExportStruct
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ExportStruct )
    TArray<FExportPanel> Panels;
};
