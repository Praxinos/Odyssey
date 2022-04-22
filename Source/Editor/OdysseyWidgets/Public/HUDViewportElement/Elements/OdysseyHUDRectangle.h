// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

#include "OdysseyHUDRectangle.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUDRectangle
UCLASS()
class ODYSSEYWIDGETS_API UOdysseyHUDRectangle : public UOdysseyHUDElement
{
    GENERATED_BODY()

public:
    void Init( FName iName, FVector2D iTopLeftPoint, FVector2D iBottomRightPoint, FTransform2D iTransform = FTransform2D() );

//UOdysseyHUDElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

public:
    UPROPERTY( EditAnywhere )
    FVector2D mTopLeftPoint;

    UPROPERTY( EditAnywhere )
    FVector2D mBottomRightPoint;

private:
    FVector2D mPreviousTopLeftPoint;
    FVector2D mPreviousBottomRightPoint;

private:
    /** The widget representation of the Rectangle in Editor */
    TSharedPtr<IDetailsView> mDetailsView;

};
