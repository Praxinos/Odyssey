// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDCircle
class ODYSSEYWIDGETS_API FOdysseyHUDCircle : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDCircle();

    //Constructor
    FOdysseyHUDCircle( FName iName, FVector2D iCenterPoint, FVector2D iBorderPoint);
    FOdysseyHUDCircle( FName iName, FVector2D iCenterPoint, float iRadius);

public:
    virtual void Render(const FOdysseyHUDSystem::FRenderParams& iParams) override;

public:
    void SetCenter(const FVector2D iCenterPoint);
    void SetRadius(float iRadius);

private:
    FVector2D mCenterPoint;
    float mRadius;
    FVector2D mBorderPoint;
};