// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDBezier
class ODYSSEYHUDSYSTEM_API FOdysseyHUDBezier : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDBezier();

    //Constructor
    FOdysseyHUDBezier(FName iName, FVector2D iStartPoint, FVector2D iEndPoint, FVector2D iControlPoint);

public:
    virtual void DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams) override;

public:
    FVector2D mStartPoint;
    FVector2D mEndPoint;
    FVector2D mControlPoint;
};
