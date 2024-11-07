// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDEllipse
class ODYSSEYHUDSYSTEM_API FOdysseyHUDEllipse : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDEllipse();

    //Constructor
    FOdysseyHUDEllipse( const FVector2D& iCenter, int iXRadius, int iYRadius);

public:
    virtual void DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams) override;

public:
    void SetCenter(const FVector2D& iCenter);
    void SetXRadius(int iRadius);
    void SetYRadius(int iRadius);

    const FVector2D& GetCenter() const;
    int GetXRadius() const;
    int GetYRadius() const;

private:
    FVector2D mCenter;
    int mXRadius;
    int mYRadius;
};