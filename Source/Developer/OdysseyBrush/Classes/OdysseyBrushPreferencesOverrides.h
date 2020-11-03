// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyStrokeOptions.h"
#include "Proxies/OdysseyBrushBlending.h"
#include "OdysseyBrushPreferencesOverrides.generated.h"

/////////////////////////////////////////////////////
// FOdysseyPerformanceOptions
USTRUCT()
struct ODYSSEYBRUSH_API FOdysseyBrushPreferencesOverrides
{
    GENERATED_BODY()

    FOdysseyBrushPreferencesOverrides()
        : bOverride_Size        ( false                                     )
        , bOverride_Opacity     ( false                                     )
        , bOverride_Flow        ( false                                     )
        , bOverride_BlendingMode( false                                     )
        , bOverride_AlphaMode   ( false                                     )
        , bOverride_Step        ( false                                     )
        , bOverride_Adaptative  ( false                                     )
        , bOverride_PaintOnTick ( false                                     )
        , bOverride_Type        ( false                                     )
        , bOverride_Method      ( false                                     )
        , bOverride_Strength    ( false                                     )
        , bOverride_Enabled     ( false                                     )
        , bOverride_RealTime    ( false                                     )
        , bOverride_CatchUp     ( false                                     )
        , Size                  ( 20                                        )
        , Opacity               ( 100                                       )
        , Flow                  ( 100                                       )
        , BlendingMode          ( EOdysseyBlendingMode::kNormal             )
        , AlphaMode             ( EOdysseyAlphaMode::kNormal                )
        , Step                  ( 20                                        )
        , SizeAdaptative        ( true                                      )
        , PaintOnTick           ( false                                     )
        , Type                  ( EOdysseyInterpolationType::kCatmullRom    )
        , Method                ( EOdysseySmoothingMethod::kAverage         )
        , Strength              ( 10                                        )
        , Enabled               ( true                                      )
        , RealTime              ( true                                      )
        , CatchUp               ( true                                      )
    {}

    /////////////////////////////////////////////////////
    // Enable Overrides
    /** Enable Modifier Size Override. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_Size;

    /** Enable Modifier Opacity Override. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_Opacity;

    /** Enable Modifier Flow Override. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_Flow;

    /** Enable Modifier BlendingMode Override. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_BlendingMode;

    /** Enable Modifier AlphaMode Override. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_AlphaMode;

    /** Enable Stroke Step Override. */
    UPROPERTY( EditAnywhere, Category = "Stroke", meta=(InlineEditConditionToggle) )
    bool    bOverride_Step;

    /** Enable Stroke Adaptative Override. */
    UPROPERTY( EditAnywhere, Category = "Stroke", meta=(InlineEditConditionToggle) )
    bool    bOverride_Adaptative;

    /** Enable Stroke PaintOnTick Override. */
    UPROPERTY( EditAnywhere, Category = "Stroke", meta=(InlineEditConditionToggle) )
    bool    bOverride_PaintOnTick;

    /** Enable Interpolation Type Override. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta=(InlineEditConditionToggle) )
    bool    bOverride_Type;

    /** Enable Smoohting Method Override. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta=(InlineEditConditionToggle) )
    bool    bOverride_Method;

    /** Enable Smoothing Strength Override. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta=(InlineEditConditionToggle) )
    bool    bOverride_Strength;

    /** Enable Smoothing Enabled Override. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta=(InlineEditConditionToggle) )
    bool    bOverride_Enabled;

    /** Enable Smoothing RealTime Override. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta=(InlineEditConditionToggle) )
    bool    bOverride_RealTime;

    /** Enable Smoothing CatchUp Override. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta=(InlineEditConditionToggle) )
    bool    bOverride_CatchUp;


    /////////////////////////////////////////////////////
    // Overrides Values
    /** Modifier Size Override Value. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta = ( ClampMin = "1", ClampMax = "1000", UIMin = "1", UIMax = "1000", SliderExponent = "1", editcondition = "bOverride_Size" ) )
    uint32  Size;

    /** Modifier Opacity Override Value. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", SliderExponent = "1", editcondition = "bOverride_Opacity" ) )
    uint32  Opacity;

    /** Modifier Flow Override Value. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", SliderExponent = "1", editcondition = "bOverride_Flow" ) )
    uint32  Flow;

    /** Modifier BlendingMode Override Value. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta = ( editcondition = "bOverride_BlendingMode" ) )
    EOdysseyBlendingMode    BlendingMode;

    /** Modifier AlphaMode Override Value. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta = ( editcondition = "bOverride_AlphaMode" ) )
    EOdysseyAlphaMode       AlphaMode;

    /** Stroke Step Override Value. */
    UPROPERTY( EditAnywhere, Category = "Stroke", meta = ( ClampMin = "1", ClampMax = "200", UIMin = "1", UIMax = "200", SliderExponent = "1", editcondition = "bOverride_Step" ) )
    int32   Step;

    /** Stroke Size Adaptative Override Value. */
    UPROPERTY( EditAnywhere, Category = "Stroke", meta = ( editcondition = "bOverride_Adaptative" ) )
    bool    SizeAdaptative;

    /** Stroke PaintOnTick Override Value. */
    UPROPERTY( EditAnywhere, Category = "Stroke", meta = ( editcondition = "bOverride_PaintOnTick" ) )
    bool    PaintOnTick;

    /** Interpolation Type Override Value. */
    UPROPERTY( EditAnywhere, Category = "Interpolation", meta = ( editcondition = "bOverride_Type" ) )
    EOdysseyInterpolationType   Type;

    /** Smoohting Method Override Value. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta = ( editcondition = "bOverride_Method" ) )
    EOdysseySmoothingMethod   Method;

    /** Smoothing Strength Override Value. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta = ( ClampMin = "0", ClampMax = "200", UIMin = "0", UIMax = "200", SliderExponent = "1", editcondition = "bOverride_Strength" ) )
    int32   Strength;

    /** Smoothing Enabled Override Value. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta = ( editcondition = "bOverride_Enabled" ) )
    bool    Enabled;

    /** Smoothing RealTime Override Value. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta = ( editcondition = "bOverride_RealTime" ) )
    bool    RealTime;

    /** Smoothing CatchUp Override Value. */
    UPROPERTY( EditAnywhere, Category = "Smoothing", meta = ( editcondition = "bOverride_CatchUp" ) )
    bool    CatchUp;

};

