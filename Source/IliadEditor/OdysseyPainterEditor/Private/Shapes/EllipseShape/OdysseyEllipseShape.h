// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"

#include "OdysseyEllipseShape.generated.h"

class FOdysseyHUDEllipse;
class FOdysseyHUDHandle;

UCLASS(meta=(DisplayName="Ellipse Shape"))
class UOdysseyEllipseShape : public UOdysseyShape
{
    GENERATED_UCLASS_BODY()

public:
    // Destructor
    virtual ~UOdysseyEllipseShape();

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);

    virtual void Abort() override;

private:
    TArray<FOdysseyPoint> GeneratePoints() const;
    void CreateHUD();
    void RefreshHUD();
    void RemoveHUD();

private:
    FOdysseyPoint mCenterPoint;
    FOdysseyPoint mBorderPoint;

    TSharedPtr<FOdysseyHUDEllipse> mEllipseHUD;

    bool mInvertUniform = false;
    bool mIsDrawing = false;

public:
    UPROPERTY(EditAnywhere, Category="Shape")
    bool Uniform = false;
};
