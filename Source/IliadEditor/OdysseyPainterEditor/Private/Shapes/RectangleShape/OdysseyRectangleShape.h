// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"

#include "OdysseyRectangleShape.generated.h"

class FOdysseyHUDRectangle;
class FOdysseyHUDHandle;

UCLASS(meta=(DisplayName="Rectangle Shape"))
class UOdysseyRectangleShape : public UOdysseyShape
{
    GENERATED_UCLASS_BODY()

public:
    // Destructor
    virtual ~UOdysseyRectangleShape();

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);

    virtual void Abort() override;

private:
    void CreateHUD();
    void RefreshHUD();
    void RemoveHUD();

private:
    FOdysseyPoint mTopLeftPoint;
    FOdysseyPoint mBottomRightPoint;

    TSharedPtr<FOdysseyHUDRectangle> mRectangleHUD;
    TSharedPtr<FOdysseyHUDHandle> mHandleTopLeftHUD;
    TSharedPtr<FOdysseyHUDHandle> mHandleBottomRightHUD;

    bool mIsDrawing = false;
    bool mInvertUniform = false;

public:
    UPROPERTY(EditAnywhere, Category="Shape")
    bool Uniform = false;
};
