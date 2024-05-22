// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDLine
class ODYSSEYWIDGETS_API FOdysseyHUDLine : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDLine();

    //Constructor
    FOdysseyHUDLine( FName iName, FVector2D iStartPoint, FVector2D iFinishPoint);

public:
    virtual void Render(const FOdysseyHUDSystem::FRenderParams& iParams) override;

public:
    FVector2D mStartPoint;
    FVector2D mFinishPoint;
};
