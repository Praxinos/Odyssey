// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>

struct FOdysseyCanvasZone
{
    TArray<FVector2D> mPolygonZone;
    ::ULIS::FBlock* mBlock;
};

class ODYSSEYIMAGING_API FOdysseyMask 
{
public:
    // Construction / Destruction
    ~FOdysseyMask();
    FOdysseyMask();

private:
    TArray<FOdysseyCanvasZone> mMaskZones;
};
