// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDLine
class ODYSSEYHUD_API FOdysseyHUDLine : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDLine();

    //Constructor
    FOdysseyHUDLine(const FVector2D& iStartPoint, const FVector2D& iEndPoint);

public:
    virtual void DrawHUD(const FOdysseyHUD::FDrawHUDParams& iParams) override;

public:
    void SetStartPoint(const FVector2D& iPoint);
    void SetEndPoint(const FVector2D& iPoint);
    const FVector2D& GetStartPoint() const;
    const FVector2D& GetEndPoint() const;

private:
    FVector2D mStartPoint;
    FVector2D mEndPoint;
};
