// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include <ULIS>
#include "ULISLoaderModule.h"

class FOdysseySurfaceTexture2DEditable;

class ODYSSEYHUDSYSTEM_API FOdysseyHUDSystem 
{
public:
    struct FDrawHUDParams
    {
        DECLARE_DELEGATE_RetVal_OneParam(FVector2D, FTextureToHUD, const FVector2D&)

        FCanvas* mCanvas;
        FTextureToHUD mTextureToHUD;
        int32 mTextureWidth;
        int32 mTextureHeight;
    };
    
    DECLARE_DELEGATE_OneParam(FOnDrawHUD, const FDrawHUDParams&)

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
    void RebuildHUDSurface(FVector2D iSize);
    void ClearHUDSurface();
    void DrawHUD( const FDrawHUDParams& iParams );
    FOnDrawHUD& OnDrawHUD();

private:
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mHUDBlock; // Holds the block in which we draw the HUD
    FOdysseySurfaceTexture2DEditable*    mHUDSurface;
    FOnDrawHUD mOnDrawHUD;
};
