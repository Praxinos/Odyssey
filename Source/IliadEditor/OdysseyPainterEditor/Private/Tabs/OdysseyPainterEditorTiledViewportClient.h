// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "RenderCommandFence.h"
#include "UObject/GCObject.h"
#include "ViewportClient.h"

#include "OdysseyVirtualRenderTarget.h"

class UTexture;
class UTexture2D;

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTiledViewportClient
class FOdysseyPainterEditorTiledViewportClient
    : public FViewportClient
    , public FGCObject
{
public:
    // Construction / Destruction
    FOdysseyPainterEditorTiledViewportClient();
    ~FOdysseyPainterEditorTiledViewportClient();

public:
    float GetZoom() const;
    void SetZoom(FViewport* iViewport, float iZoom, const FVector2D& iZoomPosition);
    void SetTextureRenderer(class IOdysseyTextureRenderingAbility* InRenderer);
    void SetCanvasSize(int InWidth, int InHeight);

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
    void DrawCheckboard( FViewport* iViewport, FCanvas* ioCanvas );
    void DrawVirtualTexture( FViewport* iViewport, FCanvas* ioCanvas );
    void DrawVirtualTextureWarning( FViewport* iViewport, FCanvas* ioCanvas );

private:
    // Private Data Members
    uint32 mCanvasWidth;
    uint32 mCanvasHeight;
    TObjectPtr<UTexture2D> mCheckerboardTexture;

    //TUniquePtr<class FTexture> mNearestNeighborTexture;
    //TUniquePtr<class FTexture> mBilinearTexture;

    bool mTranformInitialized = false;
    FMatrix mTransform;
    bool mIsPanning = false;
    float mMinZoom;
    FMatrix mInitialTransform;
    FVector2D mInitialMousePosition;

    class IOdysseyTextureRenderingAbility* mTextureRenderer = nullptr;

    TSharedPtr<class FOdysseyVirtualRenderTargetData> RenderTargetData;
    FOdysseyVirtualRenderTargetResource RenderTargetResource;

    /** Release fence to know when resources have been freed on the rendering thread. */
    FRenderCommandFence ReleaseFence;
};
