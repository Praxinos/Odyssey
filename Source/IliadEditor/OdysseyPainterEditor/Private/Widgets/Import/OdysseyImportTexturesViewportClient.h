// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyImportTexturesViewportClient
class FOdysseyImportTexturesViewportClient
    : public FViewportClient
    , public FGCObject
{
public:
    // Construction / Destruction
    FOdysseyImportTexturesViewportClient(uint32 iCanvasWidth, uint32 iCanvasHeight);
    ~FOdysseyImportTexturesViewportClient();

public:
    void SetTexture(UTexture* iTexture);
    TSharedRef<FOdysseyHUDElement> GetHUD() const;

public:
    // FViewportClient API
    virtual void Draw( FViewport* iViewport, FCanvas* ioCanvas ) override;
    virtual UWorld* GetWorld() const override { return nullptr; }

private:
    // FGCObject API
    virtual void AddReferencedObjects( FReferenceCollector& ioCollector ) override;
    virtual FString GetReferencerName() const override;

private:
    // Private Data Members
    uint32 mCanvasWidth;
    uint32 mCanvasHeight;
    TObjectPtr<UTexture2D> mCheckerboardTexture;
    TObjectPtr<UTexture> mTexture;
    TSharedRef<FOdysseyHUDElement> mHUD;
};
