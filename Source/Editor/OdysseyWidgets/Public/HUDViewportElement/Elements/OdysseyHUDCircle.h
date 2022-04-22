// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

#include "OdysseyHUDCircle.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUDCircle
UCLASS()
class ODYSSEYWIDGETS_API UOdysseyHUDCircle : public UOdysseyHUDElement
{
    GENERATED_BODY()

public:
    void Init( FName iName, FVector2D iCenterPoint, FVector2D iBorderPoint, FTransform2D iTransform = FTransform2D() );
    void Init( FName iName, FVector2D iCenterPoint, int iRadius, FTransform2D iTransform = FTransform2D() );

//UOdysseyHUDElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

public:
    UPROPERTY( EditAnywhere )
    FVector2D mCenterPoint;

    UPROPERTY( EditAnywhere )
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