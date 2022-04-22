// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include <ULIS>
#include "ULISLoaderModule.h"

class ODYSSEYHUDSYSTEM_API FOdysseyHUDToolSystem 
{
public:
    // Construction / Destruction
    ~FOdysseyHUDToolSystem();
    FOdysseyHUDToolSystem();

private:
    // Setters (private)
    void SetHUDBlock(::ULIS::FBlock* iBlock);

public:
    // Getters
    ::ULIS::FBlock* GetHUDBlock() const;
    FOdysseySurfaceTexture2DEditable* GetHUDSurface() const;

public:
    // Callback usage
    void RefreshHUDSurface(FVector2D iSize);

protected:

    ::ULIS::FBlock*                      mHUDBlock; // Holds the block in which we draw the HUD
    FOdysseySurfaceTexture2DEditable*    mHUDSurface;
};
