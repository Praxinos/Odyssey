// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"

#include "OdysseyEllipseShape.generated.h"

class FOdysseyHUDEllipse;
class FOdysseyHUDHandle;

UCLASS(meta=(DisplayName="Ellipse Shape"))
class ODYSSEYSHAPES_API UOdysseyEllipseShape : public UOdysseyShape
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

    bool mUniform = false;
    bool mIsDrawing = false;
};
