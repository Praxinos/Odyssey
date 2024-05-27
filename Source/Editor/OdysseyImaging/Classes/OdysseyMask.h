// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>

struct FOdysseyCanvasZone
{
    TArray<FVector2D> mPolygonPoints;
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlock; //We have the ownership of those blocks
};

class ODYSSEYIMAGING_API FOdysseyMask 
{
public:
    // Construction / Destruction
    ~FOdysseyMask();
    FOdysseyMask();

    void AddFromPointsAndBlock( TArray<FVector2D> iPoints, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock );
    void ClearMask();

    ::ULIS::FRectI GetMaskBoundingRect();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetMaskBlock();

private:
    TArray<FOdysseyCanvasZone> mMaskZones;
};
