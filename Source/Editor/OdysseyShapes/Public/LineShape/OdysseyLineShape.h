// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"

#include "OdysseyLineShape.generated.h"

class FOdysseyHUDLine;
class FOdysseyHUDHandle;

UCLASS(meta=(DisplayName="Line Shape"))
class ODYSSEYSHAPES_API UOdysseyLineShape : public UOdysseyShape
{
    GENERATED_UCLASS_BODY()

public:
    // Destructor
    virtual ~UOdysseyLineShape();

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);
    
    virtual void Abort() override;

private:
    void CreateHUD();
    void RefreshHUD();
    void RemoveHUD();

private:
    FOdysseyPoint mStartPoint;
    FOdysseyPoint mEndPoint;

    TSharedPtr<FOdysseyHUDLine> mLineHUD;
    TSharedPtr<FOdysseyHUDHandle> mHandleStartHUD;
    TSharedPtr<FOdysseyHUDHandle> mHandleEndHUD;

    bool mIsDrawing = false;
    bool mInvertSnapAngles = false;

public:
	UPROPERTY(EditAnywhere, Category="Shape")
	bool SnapAngles = false;
};
