// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "ExportImageSequenceStruct.generated.h"

//---

USTRUCT()
struct FExportImageSequencePanel
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, Category=ExportImageSequenceStruct )
    FFrameNumber GlobalFrame;
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
