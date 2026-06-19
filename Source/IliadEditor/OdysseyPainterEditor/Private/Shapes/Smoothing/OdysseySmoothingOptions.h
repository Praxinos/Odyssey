// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseySmoothingTypes.h"
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
    UPROPERTY(
        EditAnywhere,
        Category="Smoothing",
        meta=(
            Tooltip="The method used for smoothing.",
            DisplayName="Method"
        ) )
    EOdysseySmoothingMethod SmoothingMethod;

    /** Distance for Smooting computation. */
    UPROPERTY(
        EditAnywhere,
        Category="Smoothing",
        meta = (
            Tooltip = "Strength / Distance used by the Smooting method.",
            DisplayName="Strength",
            ClampMin = "1",
            UIMin = "1",
            LinearDeltaSensitivity = "15",
            Delta= "1"
        ) )
    int32   SmoothingStrength;

    UPROPERTY(
        EditAnywhere,
        Category="Smoothing",
        meta=(
            Tooltip="If checked, the smoothing is immediately applied while drawing, applying a distance between the cursor and what is drawn. If unchecked, the smootning is applied when the stroke is complete.",
            DisplayName="Real Time"
        ) )
    bool    SmoothingRealTime;

    /** Should smoothing catch-up to the cursor before releasing the stroke. */
    UPROPERTY(
        EditAnywhere,
        Category="Smoothing",
        meta = (
            Tooltip = "If checked, the smoothed stroke will try to catch-up to the cursor if it stops moving.",
            DisplayName="CatchUp",
            EditCondition = "SmoothingMethod == EOdysseySmoothingMethod::kAverage",
            EditConditionHides
        )
    )
    bool    SmoothingCatchUp;
};
