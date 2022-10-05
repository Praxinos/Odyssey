// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

#include "OdysseyHUDEllipse.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUDCircle
UCLASS()
class ODYSSEYWIDGETS_API UOdysseyHUDEllipse : public UOdysseyHUDElement
{
    GENERATED_BODY()

public:
    void Init( FName iName, FVector2D iCenterPoint, FVector2D iEndPoint, FTransform2D iTransform = FTransform2D() );
    void Init( FName iName, FVector2D iCenterPoint, int iEllipseAaxis, int EllipseBaxis, FTransform2D iTransform = FTransform2D() );

//UOdysseyHUDElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

public:
    UPROPERTY( EditAnywhere, Category="Odyssey HUD Ellipse" )
    FVector2D mCenterPoint;

    UPROPERTY( EditAnywhere, Category="Odyssey HUD Ellipse" )
    int mEllipseAaxis;

    UPROPERTY( EditAnywhere, Category="Odyssey HUD Ellipse" )
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