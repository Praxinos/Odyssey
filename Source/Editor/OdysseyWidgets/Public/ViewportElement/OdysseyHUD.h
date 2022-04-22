// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseyHUD.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUD
UCLASS( Abstract )
class ODYSSEYWIDGETS_API UOdysseyHUD : public UObject
{
public:
    GENERATED_BODY()

public:
    virtual TSharedPtr<SWidget> CreateWidget() PURE_VIRTUAL( CreateWidget, return SNullWidget::NullWidget; );

public:
    UPROPERTY()
    FName mName;
};
