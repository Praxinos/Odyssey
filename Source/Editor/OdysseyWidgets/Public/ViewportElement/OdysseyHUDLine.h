// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUD.h"
#include "IOdysseyViewportElement.h"

#include "OdysseyHUDLine.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUDLine
UCLASS()
class ODYSSEYWIDGETS_API UOdysseyHUDLine : public UOdysseyHUD, 
                                           public IOdysseyViewportElement
{
public:
    GENERATED_BODY()

    UOdysseyHUDLine(const FObjectInitializer& ObjectInitializer);

//UOdysseyHUD overrides
public:
    TSharedPtr<SWidget> CreateWidget() override;

//IOdysseyViewportElement overrides
public:
    void Draw(FViewport* iViewport, FCanvas* ioCanvas) override;

public:
    UPROPERTY( EditAnywhere )
    FVector2D mStartPoint;

    UPROPERTY( EditAnywhere )
    FVector2D mFinishPoint;

};
