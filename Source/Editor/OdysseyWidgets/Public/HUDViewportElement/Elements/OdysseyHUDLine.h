// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"

#include "OdysseyHUDLine.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUDLine
UCLASS()
class ODYSSEYWIDGETS_API UOdysseyHUDLine : public UOdysseyHUDElement
{
    GENERATED_BODY()

public:
    void Init( FName iName, FVector2D iStartPoint, FVector2D iFinishPoint, FTransform2D iTransform = FTransform2D() );

//UOdysseyHUDElement overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;
    void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

public:
    UPROPERTY( EditAnywhere, Category="Odyssey HUD Line" )
    FVector2D mStartPoint;

    UPROPERTY( EditAnywhere, Category="Odyssey HUD Line" )
    FVector2D mFinishPoint;

private:
    FVector2D mPreviousStartPoint;
    FVector2D mPreviousFinishPoint;

private:
    /** The widget representation of the line in Editor */
    TSharedPtr<IDetailsView> mDetailsView;

};
