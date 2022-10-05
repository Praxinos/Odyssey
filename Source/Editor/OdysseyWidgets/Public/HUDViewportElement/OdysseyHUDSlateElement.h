// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyHUDSlateElement.generated.h"

/////////////////////////////////////////////////////
// UOdysseyHUD
UCLASS( Abstract )
class ODYSSEYWIDGETS_API UOdysseyHUDSlateElement : public UObject
{
public:
    GENERATED_BODY()

public:
    virtual TSharedPtr<SWidget> CreateWidget() PURE_VIRTUAL( CreateWidget, return SNullWidget::NullWidget; );

public:
    UPROPERTY()
    FName mName;
};
