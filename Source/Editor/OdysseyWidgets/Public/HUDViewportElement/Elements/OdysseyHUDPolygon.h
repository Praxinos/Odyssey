// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDPolygon
class ODYSSEYWIDGETS_API FOdysseyHUDPolygon : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDPolygon();

    //Constructor
    FOdysseyHUDPolygon( FName iName, TArray<FVector2D> iPoints, FTransform2D iTransform = FTransform2D() );

//FOdysseyHUDElement overrides
public:
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

public:
    TArray<FVector2D>& GetPoints();

public:
    TArray<FVector2D> mPoints;

private:
    TArray<FVector2D> mPreviousPoints;

private:
    /** The widget representation of the polygon in Editor */
    TSharedPtr<IDetailsView> mDetailsView;

};
