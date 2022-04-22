// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyBrushInterpolationTypes.h"
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
    // Getters
    TSharedPtr<IOdysseyInterpolation> GetInterpolator();

public:
    // Delegates
    FOnPropertyChanged& OnPropertyChangedDelegate() { return mOnPropertyChangedDelegate; }

public:
    UPROPERTY( EditInstanceOnly, BlueprintReadOnly, meta = ( ClampMin = "1", ClampMax = "2000", UIMin = "1", UIMax = "2000", SliderExponent = "2", Delta = "1", Multiple="1" ) )
    float   Size = 20.f;

    UPROPERTY( EditInstanceOnly, BlueprintReadOnly, meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple="1", Units="Percent" ) )
    float   Flow = 100.f;

    UPROPERTY( EditInstanceOnly, BlueprintReadOnly)
    bool    SizeAdaptative = false;

    UPROPERTY( EditInstanceOnly, BlueprintReadOnly, meta = ( ClampMin = "1", ClampMax = "200", UIMin = "1", UIMax = "200", SliderExponent = "1" ) )
    float   Step = 20.f;

    UPROPERTY( EditInstanceOnly, BlueprintReadOnly)
    EOdysseyBrushInterpolationType InterpolationType = EOdysseyBrushInterpolationType::kCatmullRom;

    UPROPERTY( EditInstanceOnly, BlueprintReadOnly)
    FOdysseyBrushColor Color;

private:
    // Delegates
    FOnPropertyChanged mOnPropertyChangedDelegate;

    // ---

    TSharedPtr<IOdysseyInterpolation> mInterpolator;
};
