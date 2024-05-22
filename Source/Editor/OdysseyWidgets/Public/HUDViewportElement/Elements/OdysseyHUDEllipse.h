// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDEllipse
class ODYSSEYWIDGETS_API FOdysseyHUDEllipse : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDEllipse();

    //Constructor
    FOdysseyHUDEllipse( FName iName, FVector2D iCenterPoint, FVector2D iEndPoint);
    FOdysseyHUDEllipse( FName iName, FVector2D iCenterPoint, int iEllipseAaxis, int iEllipseBaxis);

public:
    virtual void Render(const FOdysseyHUDSystem::FRenderParams& iParams) override;

    int GetAAxis();
    int GetBAxis();

public:
    FVector2D mCenterPoint;
    int mEllipseAaxis;
    int mEllipseBaxis;
    FVector2D mBorderPoint;
};