// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "ViewportClient.h"

#include "OdysseyHUDElement.h"

class UTexture;
class UTexture2D;

/////////////////////////////////////////////////////
// FOdysseyImportTexturesViewportClient
class FOdysseyImportTexturesViewportClient
    : public FViewportClient
    , public FGCObject
{
public:
    // Construction / Destruction
    FOdysseyImportTexturesViewportClient(uint32 iCanvasWidth, uint32 iCanvasHeight, uint32 iMaxWidth, uint32 iMaxHeight);
    ~FOdysseyImportTexturesViewportClient();

public:
    void SetTexture(UTexture* iTexture);
    TSharedRef<FOdysseyHUDElement> GetHUD() const;
    float GetZoom() const;
    void SetZoom(float iZoom, const FVector2D& iZoomPosition);

public:
    // FViewportClient API
    virtual void Draw( FViewport* iViewport, FCanvas* ioCanvas ) override;
    virtual UWorld* GetWorld() const override { return nullptr; }
    virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
    virtual void CapturedMouseMove( FViewport* InViewport, int32 InMouseX, int32 InMouseY ) override;

private:
    // FGCObject API
    virtual void AddReferencedObjects( FReferenceCollector& ioCollector ) override;
    virtual FString GetReferencerName() const override;

private:
    void InitTransform(FViewport* InViewport);

private:
    // Private Data Members
    uint32 mCanvasWidth;
    uint32 mCanvasHeight;
    uint32 mMaxWidth;
    uint32 mMaxHeight;
    TObjectPtr<UTexture2D> mCheckerboardTexture;
    TObjectPtr<UTexture> mTexture;
    TSharedRef<FOdysseyHUDElement> mHUD;

    TUniquePtr<class FTexture> mNearestNeighborTexture;
    TUniquePtr<class FTexture> mBilinearTexture;

    bool mTranformInitialized = false;
    FMatrix mTransform;
    bool mIsPanning = false;
    bool mIsZooming = false;
    float mMinZoom;
    FMatrix mInitialTransform;
    FVector2D mInitialMousePosition;
};
