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
    struct FRenderParams
    {
        const FSceneView* mView;
        FViewport* mViewport;
        FPrimitiveDrawInterface* mPDI;
        FVector mOrigin;
        FVector mXAxis;
        FVector mYAxis;
        double mPlaneWidth;
        double mPlaneHeight;
    };
    
    DECLARE_DELEGATE_OneParam(FOnRender, const FRenderParams&)

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
    void Render( const FRenderParams& iParams );
    FOnRender& OnRender();

private:
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mHUDBlock; // Holds the block in which we draw the HUD
    FOdysseySurfaceTexture2DEditable*    mHUDSurface;
    FOnRender mOnRender;
};
