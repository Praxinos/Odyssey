// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDPolygon
class ODYSSEYHUD_API FOdysseyHUDPolygon : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDPolygon();

    //Constructor
    FOdysseyHUDPolygon();

//FOdysseyHUDElement overrides
public:
    virtual void DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams) override;

public:
    TArray<FVector2D>& GetPoints();
    void ClosePolygon(bool iClosePolygon);

private:
    TArray<FVector2D> mPoints;
    bool mClosePolygon = true;
};
