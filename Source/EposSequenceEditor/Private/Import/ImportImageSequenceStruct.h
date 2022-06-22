// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "ImportImageSequenceStruct.generated.h"

//---

USTRUCT()
struct FImportImageSequencePanel
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct, DisplayName="Panel Id" )
    FString Id;

    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct )
    FFilePath Pathfile;

    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct )
    int32 Duration { 48 };
};

//---

USTRUCT()
struct FImportImageSequenceShot
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct, DisplayName="Shot Id" )
    FString Id;

    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct )
    TArray<FImportImageSequencePanel> Panels;
};

//---

USTRUCT()
struct FImportImageSequenceBoard
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct, DisplayName="Board Id" )
    FString Id;

    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct )
    TArray<FImportImageSequenceShot> Shots;
};

//---

USTRUCT()
struct FImportImageSequenceStruct
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct )
    TArray<FImportImageSequenceBoard> Boards;
};
