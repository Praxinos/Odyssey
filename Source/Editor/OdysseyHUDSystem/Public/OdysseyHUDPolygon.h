// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDPolygon
class ODYSSEYHUDSYSTEM_API FOdysseyHUDPolygon : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDPolygon();

    //Constructor
    FOdysseyHUDPolygon( FName iName);

//FOdysseyHUDElement overrides
public:
    virtual void DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams) override;

public:
    TArray<FVector2D>& GetPoints();
    void ClosePolygon(bool iClosePolygon);

private:
    TArray<FVector2D> mPoints;
    bool mClosePolygon = true;
};
