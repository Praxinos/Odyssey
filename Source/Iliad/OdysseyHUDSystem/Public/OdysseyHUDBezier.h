// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDBezier
class ODYSSEYHUDSYSTEM_API FOdysseyHUDBezier : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDBezier();

    //Constructor
    FOdysseyHUDBezier(const FVector2D& iStartPoint, const FVector2D& iEndPoint, const FVector2D& iControlPoint);

public:
    virtual void DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams) override;

public:
    void SetStartPoint(const FVector2D& iPoint);
    void SetEndPoint(const FVector2D& iPoint);
    void SetControlPoint(const FVector2D& iPoint);

    const FVector2D& GetStartPoint() const;
    const FVector2D& GetEndPoint() const;
    const FVector2D& GetControlPoint() const;

private:
    FVector2D mStartPoint;
    FVector2D mEndPoint;
    FVector2D mControlPoint;
};
