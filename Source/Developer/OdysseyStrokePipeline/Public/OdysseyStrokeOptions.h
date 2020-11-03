// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseySmoothingTypes.h"
#include "OdysseyInterpolationTypes.h"
#include "OdysseyStrokeOptions.generated.h"

USTRUCT()
struct ODYSSEYSTROKEPIPELINE_API FOdysseyStrokeOptions
{
    GENERATED_BODY()

    FOdysseyStrokeOptions()
        : Step          ( 20 )
        , SizeAdaptative( true )
        , PaintOnTick   ( false )
        , Type          ( EOdysseyInterpolationType::kCatmullRom )
        , Method        ( EOdysseySmoothingMethod::kAverage )
        , Strength      ( 10 )
        , Enabled       ( true )
        , RealTime      ( true )
        , CatchUp       ( true )
    {}

    /** Distance beetween two Stamps. */
    UPROPERTY( EditAnywhere, Category = "Stroke", meta = ( ClampMin = "1", ClampMax = "200", UIMin = "1", UIMax = "200", SliderExponent = "1" ) )
    int32   Step;

    /** Should Step be expressed as a relative fraction of the result Stamp size, or in pixels. */
    UPROPERTY( EditAnywhere, Category = "Stroke" )
    bool    SizeAdaptative;

    /** Should Steps events be triggered for each tick, even if the mouse doesn't move when drawing. */
    UPROPERTY( EditAnywhere, Category = "Stroke" )
    bool    PaintOnTick;

    /** The type of interpolation. */
    UPROPERTY( EditAnywhere, Category = "Interpolation" )
    EOdysseyInterpolationType Type;

    /** The method used for smoothing. */
    UPROPERTY( EditAnywhere, Category = "Smoothing" )
    EOdysseySmoothingMethod Method;

    /** Distance for Smooting computation. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta = ( ClampMin = "1", ClampMax = "200", UIMin = "1", UIMax = "200", SliderExponent = "1" ) )
    int32   Strength;

    /** Should smoothing be enabled. */
    UPROPERTY( EditAnywhere, Category = "Smoothing" )
    bool    Enabled;

    /** Should smoothing be real-time, meaning the first input draws directly, even though the strength is not reached yet. */
    UPROPERTY( EditAnywhere, Category = "Smoothing" )
    bool    RealTime;

    /** Should smoothing catch-up to the cursor before releasing the stroke. */
    UPROPERTY( EditAnywhere, Category = "Smoothing" )
    bool    CatchUp;
};
