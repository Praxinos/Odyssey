// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDPolygon.h"
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
    void ClearMaskData();
    void ClearMaskHUD();

    ::ULIS::FRectI GetMaskBoundingRect();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetMaskBlock();
    TArray<TSharedPtr<FOdysseyHUDPolygon>>& GetMaskHUD();

    void RefreshMaskHUD();

private:
    TArray<FOdysseyCanvasZone> mMaskZones;
    TArray<TSharedPtr<FOdysseyHUDPolygon>> mMaskHUD;
};
