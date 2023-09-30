// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDEllipse
class ODYSSEYWIDGETS_API FOdysseyHUDEllipse : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDEllipse();

    //Constructor
    FOdysseyHUDEllipse( FName iName, FVector2D iCenterPoint, FVector2D iEndPoint, FTransform2D iTransform = FTransform2D() );
    FOdysseyHUDEllipse( FName iName, FVector2D iCenterPoint, int iEllipseAaxis, int iEllipseBaxis, FTransform2D iTransform = FTransform2D() );

//FOdysseyHUDElement overrides
public:
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

private:
    FVector2D mCenterPoint;
    int mEllipseAaxis;
    int mEllipseBaxis;

    FVector2D mBorderPoint;

private:
    FVector2D mPreviousCenterPoint;
    FVector2D mPreviousBorderPoint;
    int mPreviousEllipseAaxis;
    int mPreviousEllipseBaxis;

private:
    /** The widget representation of the line in Editor */
    TSharedPtr<IDetailsView> mDetailsView;

};