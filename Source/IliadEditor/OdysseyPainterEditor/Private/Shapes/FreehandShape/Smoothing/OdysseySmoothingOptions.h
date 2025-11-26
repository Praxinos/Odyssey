// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "FreehandShape/Smoothing/OdysseySmoothingTypes.h"
#include "OdysseySmoothingOptions.generated.h"

USTRUCT(Blueprintable, meta=(DisplayName="Smoothing"))
struct FOdysseySmoothingOptions
{
    GENERATED_BODY()

    FOdysseySmoothingOptions()
        : SmoothingMethod        ( EOdysseySmoothingMethod::kAverage )
        , SmoothingStrength      ( 10 )
        , SmoothingRealTime      ( true )
        , SmoothingCatchUp       ( true )
    {}

    /** The method used for smoothing. */
    UPROPERTY( EditAnywhere, Category="Smoothing", meta=(DisplayName="Method") )
    EOdysseySmoothingMethod SmoothingMethod;

    /** Distance for Smooting computation. */
    UPROPERTY( EditAnywhere, Category="Smoothing", meta = ( DisplayName="Strength", ClampMin = "1", UIMin = "1", LinearDeltaSensitivity = "15", Delta = "1", Multiple="1" ) )
    int32   SmoothingStrength;

    /** Should smoothing be real-time, meaning the first input draws directly, even though the strength is not reached yet. */
    UPROPERTY( EditAnywhere, Category="Smoothing", meta=(DisplayName="Real Time") )
    bool    SmoothingRealTime;

    /** Should smoothing catch-up to the cursor before releasing the stroke. */
    UPROPERTY( EditAnywhere, Category="Smoothing", meta = (DisplayName="CatchUp", EditCondition = "SmoothingMethod == EOdysseySmoothingMethod::kAverage", EditConditionHides) )
    bool    SmoothingCatchUp;
};
