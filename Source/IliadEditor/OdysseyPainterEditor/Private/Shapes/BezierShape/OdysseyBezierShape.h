// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"

#include "OdysseyBezierShape.generated.h"

class FOdysseyHUDBezier;
class FOdysseyHUDHandle;

UCLASS(meta=(DisplayName="Bezier Shape"))
class UOdysseyBezierShape : public UOdysseyShape
{
    GENERATED_UCLASS_BODY()
public:
    // Destructor
    virtual ~UOdysseyBezierShape();

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnKeyDown(const FKey& iKey);

    virtual void Abort() override;

private:
    TArray<FOdysseyPoint> GeneratePoints() const;
    void CommitBezier();
    void CreateHUD();
    void RefreshHUD();
    void RemoveHUD();

private:
    FOdysseyPoint mStartPoint;
    FOdysseyPoint mControlPoint;
    FOdysseyPoint mEndPoint;

    TSharedPtr<FOdysseyHUDBezier> mBezierHUD;
    TSharedPtr<FOdysseyHUDHandle> mHandleStartHUD;
    TSharedPtr<FOdysseyHUDHandle> mHandleControlHUD;
    TSharedPtr<FOdysseyHUDHandle> mHandleEndHUD;

    enum class eEventState : uint8
    {
        Idle, //nothing is happening
        EndPoint, //we are controling the endpoint
        ControlPoint // we are controling the control point
    };

    eEventState mEventState = eEventState::Idle;
};
