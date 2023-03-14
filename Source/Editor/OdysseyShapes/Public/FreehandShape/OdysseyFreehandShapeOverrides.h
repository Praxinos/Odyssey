// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "FreehandShape/Smoothing/OdysseySmoothingTypes.h"
#include "FreehandShape/OdysseyFreehandShape.h"

#include "OdysseyFreehandShapeOverrides.generated.h"

/////////////////////////////////////////////////////
// FOdysseyFreehandShapeOverrides
UCLASS(meta=(DisplayName="Freehand Shape"))
class ODYSSEYSHAPES_API UOdysseyFreehandShapeOverrides : public UObject
{
    GENERATED_BODY()

    UOdysseyFreehandShapeOverrides();

    /** Enable Smoohting Method Override. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta=(InlineEditConditionToggle) )
    bool    bOverride_SmoothingMethod;

    /** Enable Smoothing Strength Override. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta=(InlineEditConditionToggle) )
    bool    bOverride_SmoothingStrength;

    /** Enable Smoothing Enabled Override. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta=(InlineEditConditionToggle) )
    bool    bOverride_SmoothingEnabled;

    /** Enable Smoothing RealTime Override. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta=(InlineEditConditionToggle) )
    bool    bOverride_SmoothingRealTime;

    /** Enable Smoothing CatchUp Override. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta=(InlineEditConditionToggle) )
    bool    bOverride_SmoothingCatchUp;

    /** Enable Stroke Step Override. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta=(InlineEditConditionToggle) )
    bool    bOverride_Step;

    /** Enable Stroke Adaptative Override. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta=(InlineEditConditionToggle) )
    bool    bOverride_AdaptativeStep;

    /** Enable Interpolation Type Override. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta=(InlineEditConditionToggle) )
    bool    bOverride_InterpolationType;


    /////////////////////////////////////////////////////
    // Overrides Values

    /** Smoohting Method Override Value. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta = ( editcondition = "bOverride_SmoothingMethod" ) )
    EOdysseySmoothingMethod   SmoothingMethod;

    /** Smoothing Strength Override Value. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta = ( ClampMin = "0", ClampMax = "200", UIMin = "0", UIMax = "200", SliderExponent = "1", editcondition = "bOverride_SmoothingStrength" ) )
    int32   SmoothingStrength;

    /** Smoothing Enabled Override Value. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta = ( editcondition = "bOverride_SmoothingEnabled" ) )
    bool    SmoothingEnabled;

    /** Smoothing RealTime Override Value. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta = ( editcondition = "bOverride_SmoothingRealTime" ) )
    bool    SmoothingRealTime;

    /** Smoothing CatchUp Override Value. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta = ( editcondition = "bOverride_SmoothingCatchUp" ) )
    bool    SmoothingCatchUp;

    /** Stroke Step Override Value. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta = ( ClampMin = "1", ClampMax = "200", UIMin = "1", UIMax = "200", SliderExponent = "1", editcondition = "bOverride_Step" ) )
    float   Step;

    /** Stroke Size Adaptative Override Value. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta = ( editcondition = "bOverride_AdaptativeStep" ) )
    bool    AdaptativeStep;

    /** Interpolation Type Override Value. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta = ( editcondition = "bOverride_InterpolationType" ) )
    EOdysseyInterpolationType   InterpolationType;

public:
    //Applies the Overrides to the given object
    void Override(UOdysseyFreehandShape* iFreehandShape) const;
};