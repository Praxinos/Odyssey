// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDRectangle
class ODYSSEYHUDSYSTEM_API FOdysseyHUDRectangle : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDRectangle();

    //Constructor
    FOdysseyHUDRectangle( const FVector2D& iTopLeftPoint, const FVector2D& iBottomRightPoint);

public:
    virtual void DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams) override;

public:
    void SetTopLeftPoint(const FVector2D& iPoint);
    void SetBottomRightPoint(const FVector2D& iPoint);

    const FVector2D& GetTopLeftPoint() const;
    const FVector2D& GetBottomRightPoint() const;

private:
    FVector2D mTopLeftPoint;
    FVector2D mBottomRightPoint;
};
