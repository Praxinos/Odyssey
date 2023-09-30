// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDCircle
class ODYSSEYWIDGETS_API FOdysseyHUDCircle : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDCircle();

    //Constructor
    FOdysseyHUDCircle( FName iName, FVector2D iCenterPoint, FVector2D iBorderPoint, FTransform2D iTransform = FTransform2D() );
    FOdysseyHUDCircle( FName iName, FVector2D iCenterPoint, int iRadius, FTransform2D iTransform = FTransform2D() );

//FOdysseyHUDElement overrides
public:
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

private:
    FVector2D mCenterPoint;
    int mRadius;
    FVector2D mBorderPoint;

private:
    FVector2D mPreviousCenterPoint;
    FVector2D mPreviousBorderPoint;
    int mPreviousRadius;

private:
    /** The widget representation of the line in Editor */
    TSharedPtr<IDetailsView> mDetailsView;

};