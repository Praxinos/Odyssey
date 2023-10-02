// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDBezier
class ODYSSEYWIDGETS_API FOdysseyHUDBezier : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDBezier();

    //Constructor
    FOdysseyHUDBezier(FName iName, FVector2D iStartPoint, FVector2D iEndPoint, FVector2D iControlPoint, FTransform2D iTransform = FTransform2D());

//FOdysseyHUDElement overrides
public:
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

public:
    FVector2D mStartPoint;
    FVector2D mEndPoint;
    FVector2D mControlPoint;

private:
    FVector2D mPreviousStartPoint;
    FVector2D mPreviousEndPoint;
    FVector2D mPreviousControlPoint;

private:
    /** The widget representation of the Bezier in Editor */
    TSharedPtr<IDetailsView> mDetailsView;

};
