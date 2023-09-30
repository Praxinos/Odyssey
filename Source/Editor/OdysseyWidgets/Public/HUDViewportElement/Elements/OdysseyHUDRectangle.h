// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDRectangle
class ODYSSEYWIDGETS_API FOdysseyHUDRectangle : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDRectangle();

    //Constructor
    FOdysseyHUDRectangle( FName iName, FVector2D iTopLeftPoint, FVector2D iBottomRightPoint, FTransform2D iTransform = FTransform2D() );

//FOdysseyHUDElement overrides
public:
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

private:
    FVector2D mTopLeftPoint;
    FVector2D mBottomRightPoint;

private:
    FVector2D mPreviousTopLeftPoint;
    FVector2D mPreviousBottomRightPoint;

private:
    /** The widget representation of the Rectangle in Editor */
    TSharedPtr<IDetailsView> mDetailsView;

};
