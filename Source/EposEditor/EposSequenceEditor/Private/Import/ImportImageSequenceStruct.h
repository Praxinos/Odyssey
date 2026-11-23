// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "ImportImageSequenceStruct.generated.h"

//---

USTRUCT()
struct FImportImageSequencePanel
{
    GENERATED_BODY()

public:
    /** Identifier of the panel, read from the {panel} pattern. */
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct, DisplayName="Panel Id" )
    FString Id;

    /** Image file of the panel. */
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct )
    FFilePath Pathfile;

    /** Duration of the panel, in frames. */
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct )
    int32 Duration { 48 };
};

//---

USTRUCT()
struct FImportImageSequenceShot
{
    GENERATED_BODY()

public:
    /** Identifier of the shot, read from the {shot} pattern. */
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct, DisplayName="Shot Id" )
    FString Id;

    /** Panels of the shot, each one becoming a Cell. */
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct )
    TArray<FImportImageSequencePanel> Panels;
};

//---

USTRUCT()
struct FImportImageSequenceBoard
{
    GENERATED_BODY()

public:
    /** Identifier of the board, read from the {board} pattern. */
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct, DisplayName="Board Id" )
    FString Id;

    /** Shots of the board, each one becoming a Shot Sequence. */
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct )
    TArray<FImportImageSequenceShot> Shots;
};

//---

USTRUCT()
struct FImportImageSequenceStruct
{
    GENERATED_BODY()

public:
    /**
     * Boards found in the image sequence.
     *
     * Each one becomes a sub-Board Sequence in the root Board Sequence.
     */
    UPROPERTY( EditAnywhere, Category=ImportImageSequenceStruct )
    TArray<FImportImageSequenceBoard> Boards;
};
