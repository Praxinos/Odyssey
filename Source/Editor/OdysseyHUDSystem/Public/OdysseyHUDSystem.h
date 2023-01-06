// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include <ULIS>
#include "ULISLoaderModule.h"

class ODYSSEYHUDSYSTEM_API FOdysseyHUDSystem 
{
public:
    // Construction / Destruction
    ~FOdysseyHUDSystem();
    FOdysseyHUDSystem();

private:
    // Setters (private)
    void SetHUDBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock);

public:
    // Getters
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetHUDBlock() const;
    FOdysseySurfaceTexture2DEditable* GetHUDSurface() const;

public:
    // Callback usage
    void RefreshHUDSurface(FVector2D iSize);

private:
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mHUDBlock; // Holds the block in which we draw the HUD
    FOdysseySurfaceTexture2DEditable*    mHUDSurface;
};
