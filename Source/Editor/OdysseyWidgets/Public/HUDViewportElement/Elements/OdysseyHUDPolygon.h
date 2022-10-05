// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

#include "OdysseyHUDPolygon.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUDPolygon
UCLASS()
class ODYSSEYWIDGETS_API UOdysseyHUDPolygon : public UOdysseyHUDElement
{
    GENERATED_BODY()

public:
    void Init( FName iName, TArray<FVector2D> iPoints, FTransform2D iTransform = FTransform2D() );

//UOdysseyHUDElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

public:
    UPROPERTY( EditAnywhere, Category="Odyssey HUD Polygon" )
    TArray<FVector2D> mPoints;

private:
    TArray<FVector2D> mPreviousPoints;

private:
    /** The widget representation of the polygon in Editor */
    TSharedPtr<IDetailsView> mDetailsView;

};
