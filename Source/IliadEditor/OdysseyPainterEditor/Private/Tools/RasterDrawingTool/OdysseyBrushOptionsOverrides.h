// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyBrushAssetBase.h"
#include "UObject/ObjectMacros.h"
#include "Image/OdysseyBlendingMode.h"
#include "OdysseyBrushOptionsOverrides.generated.h"

/////////////////////////////////////////////////////
// FOdysseyBrushOptionsOverrides
UCLASS(meta=(DisplayName="Brush Options"))
class ODYSSEYPAINTEREDITOR_API UOdysseyBrushOptionsOverrides : public UObject
{
    GENERATED_BODY()

public:
    UOdysseyBrushOptionsOverrides();

public:
    /////////////////////////////////////////////////////
    // Enable Overrides
    /** Enable Modifier Size Override. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_Size;

    /** Enable Modifier Flow Override. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_Flow;


    /////////////////////////////////////////////////////
    // Overrides Values
    /** Modifier Size Override Value. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta = ( ClampMin = "1", ClampMax = "1000", UIMin = "1", UIMax = "1000", SliderExponent = "1", editcondition = "bOverride_Size" ) )
    float  Size;

    /** Modifier Flow Override Value. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", SliderExponent = "1", editcondition = "bOverride_Flow" ) )
    float  Flow;
};
