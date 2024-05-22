// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDRectangle
class ODYSSEYWIDGETS_API FOdysseyHUDRectangle : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDRectangle();

    //Constructor
    FOdysseyHUDRectangle( FName iName, FVector2D iTopLeftPoint, FVector2D iBottomRightPoint);

public:
    virtual void DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams) override;

public:
    FVector2D mTopLeftPoint;
    FVector2D mBottomRightPoint;
};
