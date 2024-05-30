// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"

#include "OdysseyPolygonShape.generated.h"

class FOdysseyHUDPolygon;
class FOdysseyHUDHandle;

UCLASS(meta=(DisplayName="Polygon Shape"))
class ODYSSEYSHAPES_API UOdysseyPolygonShape : public UOdysseyShape
{
    GENERATED_UCLASS_BODY()

public:
    // Destructor
    virtual ~UOdysseyPolygonShape();

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);
    
    virtual void Abort() override;

private:
    void OnFirstHandleDragEnd();
    void CommitPolygon();
    void CreateHUD();
    void RemoveHUD();
    void SetLastHUDPoint(const FOdysseyPoint& iPoint);
    void AddPointToHUD(const FOdysseyPoint& iPoint);
    void RebuildHandleHUDs();

private:
    TArray<FOdysseyPoint> mPoints;

    TSharedPtr<FOdysseyHUDPolygon> mPolygonHUD;
    TArray<TSharedPtr<FOdysseyHUDHandle>> mHandleHUDs;

    bool mIsDrawing = false;
    bool mSnapAngles = false;
};
