// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    void Init( FName iName, FVector2D iCenterPoint, FVector2D iEndPoint, int iAngle, FTransform2D iTransform = FTransform2D() );
    void Init( FName iName, FVector2D iCenterPoint, int iEllipseAaxis, int EllipseBaxis, int iAngle, FTransform2D iTransform = FTransform2D() );

//UOdysseyHUDElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

public:
    UPROPERTY( EditAnywhere )
    FVector2D mCenterPoint;

    UPROPERTY( EditAnywhere )
    FVector2D mEndPoint;

    UPROPERTY( EditAnywhere )
    int mEllipseAaxis;

    UPROPERTY( EditAnywhere )
    int mEllipseBaxis;

    UPROPERTY( EditAnywhere )
    int mAngle;

    FVector2D mBorderPoint;

private:
    FVector2D mPreviousCenterPoint;
    FVector2D mPreviousEndPoint;
    int mPreviousEllipseAaxis;
    int mPreviousEllipseBaxis;
    int mPreviousAngle;

private:
    /** The widget representation of the line in Editor */
    TSharedPtr<IDetailsView> mDetailsView;

};