// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "FreehandShape/Smoothing/OdysseySmoothingTypes.h"
#include "OdysseySmoothingOptions.generated.h"

USTRUCT(Blueprintable, meta=(DisplayName="Smoothing"))
struct ODYSSEYSHAPES_API FOdysseySmoothingOptions
{
    GENERATED_BODY()

    FOdysseySmoothingOptions()
        : SmoothingMethod        ( EOdysseySmoothingMethod::kAverage )
        , SmoothingStrength      ( 10 )
        , SmoothingRealTime      ( true )
        , SmoothingCatchUp       ( true )
    {}

    /** The method used for smoothing. */
    UPROPERTY( EditAnywhere, Category="Smoothing" )
    EOdysseySmoothingMethod SmoothingMethod;

    /** Distance for Smooting computation. */
    UPROPERTY( EditAnywhere, Category="Smoothing", meta = ( ClampMin = "1", ClampMax = "200", UIMin = "1", UIMax = "200", SliderExponent = "1" ) )
    int32   SmoothingStrength;

    /** Should smoothing be real-time, meaning the first input draws directly, even though the strength is not reached yet. */
    UPROPERTY( EditAnywhere, Category="Smoothing" )
    bool    SmoothingRealTime;

    /** Should smoothing catch-up to the cursor before releasing the stroke. */
    UPROPERTY( EditAnywhere, Category="Smoothing" )
    bool    SmoothingCatchUp;
};
