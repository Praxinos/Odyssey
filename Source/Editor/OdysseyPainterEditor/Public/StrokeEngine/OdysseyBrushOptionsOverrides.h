// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseyBrushAssetBase.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyBrushInterpolationTypes.h"
#include "Image/OdysseyBlendingMode.h"
#include "OdysseyBrushOptionsOverrides.generated.h"

/////////////////////////////////////////////////////
// FOdysseyBrushOptionsOverrides
UCLASS(meta=(DisplayName="Brush Options"))
class ODYSSEYPAINTEREDITOR_API UOdysseyBrushOptionsOverrides : public UObject
{
    GENERATED_BODY()

    UOdysseyBrushOptionsOverrides();

    /////////////////////////////////////////////////////
    // Enable Overrides
    /** Enable Modifier Size Override. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_Size;

    /** Enable Modifier Flow Override. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_Flow;

    /** Enable Stroke Step Override. */
    UPROPERTY( EditAnywhere, Category = "Stroke", meta=(InlineEditConditionToggle) )
    bool    bOverride_Step;

    /** Enable Stroke Adaptative Override. */
    UPROPERTY( EditAnywhere, Category = "Stroke", meta=(InlineEditConditionToggle) )
    bool    bOverride_Adaptative;

    /** Enable Interpolation Type Override. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta=(InlineEditConditionToggle) )
    bool    bOverride_Type;


    /////////////////////////////////////////////////////
    // Overrides Values
    /** Modifier Size Override Value. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta = ( ClampMin = "1", ClampMax = "1000", UIMin = "1", UIMax = "1000", SliderExponent = "1", editcondition = "bOverride_Size" ) )
    float  Size;

    /** Modifier Flow Override Value. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", SliderExponent = "1", editcondition = "bOverride_Flow" ) )
    float  Flow;

    /** Stroke Step Override Value. */
    UPROPERTY( EditAnywhere, Category = "Stroke", meta = ( ClampMin = "1", ClampMax = "200", UIMin = "1", UIMax = "200", SliderExponent = "1", editcondition = "bOverride_Step" ) )
    float   Step;

    /** Stroke Size Adaptative Override Value. */
    UPROPERTY( EditAnywhere, Category = "Stroke", meta = ( editcondition = "bOverride_Adaptative" ) )
    bool    SizeAdaptative;

    /** Interpolation Type Override Value. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta = ( editcondition = "bOverride_Type" ) )
    EOdysseyBrushInterpolationType   Type;

public:
    //Applies the Overrides to the given object
    void Override(UOdysseyBrushOptions* iBlendParameters) const;
};