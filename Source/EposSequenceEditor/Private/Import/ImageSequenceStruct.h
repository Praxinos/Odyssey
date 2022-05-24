// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "ImageSequenceStruct.generated.h"

//---

USTRUCT()
struct FImageSequenceFrame
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ImageSequenceStruct, DisplayName="Frame Id" )
    FString Id;

    UPROPERTY( EditAnywhere, Category=ImageSequenceStruct )
    FFilePath Pathfile;

    UPROPERTY( EditAnywhere, Category=ImageSequenceStruct )
    int32 Duration { 48 };
};

//---

USTRUCT()
struct FImageSequenceShot
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ImageSequenceStruct, DisplayName="Shot Id" )
    FString Id;

    UPROPERTY( EditAnywhere, Category=ImageSequenceStruct )
    TArray<FImageSequenceFrame> Frames;
};

//---

USTRUCT()
struct FImageSequenceBoard
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ImageSequenceStruct, DisplayName="Board Id" )
    FString Id;

    UPROPERTY( EditAnywhere, Category=ImageSequenceStruct )
    TArray<FImageSequenceShot> Shots;
};

//---

USTRUCT()
struct FImageSequenceStruct
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ImageSequenceStruct )
    TArray<FImageSequenceBoard> Boards;
};
