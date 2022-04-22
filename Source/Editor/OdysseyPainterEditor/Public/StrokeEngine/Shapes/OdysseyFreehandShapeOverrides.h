// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "StrokeEngine/Smoothing/OdysseySmoothingTypes.h"

#include "OdysseyFreehandShapeOverrides.generated.h"

/////////////////////////////////////////////////////
// FOdysseyFreehandShapeOverrides
UCLASS(meta=(DisplayName="Freehand Shape"))
class ODYSSEYPAINTEREDITOR_API UOdysseyFreehandShapeOverrides : public UObject
{
    GENERATED_BODY()

    UOdysseyFreehandShapeOverrides();

    /** Enable Smoohting Method Override. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta=(InlineEditConditionToggle) )
    bool    bOverride_SmoothingMethod;

    /** Enable Smoothing Strength Override. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta=(InlineEditConditionToggle) )
    bool    bOverride_SmoothingStrength;

    /** Enable Smoothing Enabled Override. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta=(InlineEditConditionToggle) )
    bool    bOverride_SmoothingEnabled;

    /** Enable Smoothing RealTime Override. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta=(InlineEditConditionToggle) )
    bool    bOverride_SmoothingRealTime;

    /** Enable Smoothing CatchUp Override. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta=(InlineEditConditionToggle) )
    bool    bOverride_SmoothingCatchUp;


    /////////////////////////////////////////////////////
    // Overrides Values

    /** Smoohting Method Override Value. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta = ( editcondition = "bOverride_SmoothingMethod" ) )
    EOdysseySmoothingMethod   SmoothingMethod;

    /** Smoothing Strength Override Value. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta = ( ClampMin = "0", ClampMax = "200", UIMin = "0", UIMax = "200", SliderExponent = "1", editcondition = "bOverride_SmoothingStrength" ) )
    int32   SmoothingStrength;

    /** Smoothing Enabled Override Value. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta = ( editcondition = "bOverride_SmoothingEnabled" ) )
    bool    SmoothingEnabled;

    /** Smoothing RealTime Override Value. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta = ( editcondition = "bOverride_SmoothingRealTime" ) )
    bool    SmoothingRealTime;

    /** Smoothing CatchUp Override Value. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta = ( editcondition = "bOverride_SmoothingCatchUp" ) )
    bool    SmoothingCatchUp;

public:
    //Applies the Overrides to the given object
    void Override(UOdysseyFreehandShape* iFreehandShape) const;
};