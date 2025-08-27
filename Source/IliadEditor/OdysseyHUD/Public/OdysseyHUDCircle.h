// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDCircle
class ODYSSEYHUD_API FOdysseyHUDCircle : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDCircle();

    //Constructor
    FOdysseyHUDCircle( const FVector2D& iCenterPoint, float iRadius);

public:
    virtual void DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams) override;

public:
    void SetCenter(const FVector2D& iCenterPoint);
    void SetRadius(float iRadius);

private:
    FVector2D mCenterPoint;
    float mRadius;
};
