// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Proxies/OdysseyBrushColor.h"

#include <ULIS>

#include "OdysseyBrushOptions.generated.h"

UCLASS()
class ODYSSEYBRUSH_API UOdysseyBrushOptions : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    DECLARE_MULTICAST_DELEGATE(FOnPropertyChanged);

public:
    //UObject overrides

    //Called when a simple property changes
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent & PropertyChangedEvent);

public:
    // Delegates
    FOnPropertyChanged& OnPropertyChangedDelegate() { return mOnPropertyChangedDelegate; }

public:
    UPROPERTY( EditInstanceOnly, BlueprintReadOnly, Category="Common", meta = ( ClampMin = "1", ClampMax = "2000", UIMin = "1", UIMax = "2000", SliderExponent = "2", Delta = "1", Multiple="1", DisplayPriority="1" ) )
    float   Size = 20.f;

    UPROPERTY( EditInstanceOnly, BlueprintReadOnly, Category="Common", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple="1", Units="Percent", DisplayPriority = "1") )
    float   Flow = 100.f;

    UPROPERTY( BlueprintReadOnly, Category="Common")
    FOdysseyBrushColor Color;

private:
    // Delegates
    FOnPropertyChanged mOnPropertyChangedDelegate;
};
