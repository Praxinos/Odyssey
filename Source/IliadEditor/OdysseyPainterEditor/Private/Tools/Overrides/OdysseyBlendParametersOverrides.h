// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyBlendingMode.h"
#include "OdysseyBlendMode.h"
#include "OdysseyBlendParametersOverrides.generated.h"

/////////////////////////////////////////////////////
// FOdysseyBlendParametersOverrides
UCLASS(meta=(DisplayName="Blend Parameters"))
class ODYSSEYPAINTEREDITOR_API UOdysseyBlendParametersOverrides : public UObject
{
    GENERATED_BODY()

public:
    UOdysseyBlendParametersOverrides();

    virtual void Serialize(FArchive& Ar) override;

private:
    //Deprecated values
    UPROPERTY()
    bool    bOverride_BlendingMode_DEPRECATED;
    UPROPERTY()
    bool    bOverride_AlphaMode_DEPRECATED;
    UPROPERTY()
    EOdysseyBlendingMode    BlendingMode_DEPRECATED;
    UPROPERTY()
    EOdysseyAlphaMode       AlphaMode_DEPRECATED;

public:
    /** Enable Modifier Opacity Override. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_Opacity;

    /** Enable Modifier Advanced Blend Modes Override. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_IsComposite;

    /** Enable Modifier Eraser Mode Override. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_EraserMode;

    /** Enable Modifier Blending Mode Override. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_BlendMode;

    /** Enable Modifier Color Blend Mode Override. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_CompositeColorBlendMode;

    /** Enable Modifier Alpha Blend Mode Override. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_CompositeAlphaBlendMode;

    /////////////////////////////////////////////////////
    // Overrides Values
    /** Modifier Opacity Override Value. */
    UPROPERTY()
    float  Opacity;

    /**
     * Modifier Eraser Mode Override Value.
     *
     * If checked, the brush is applied as an eraser when it is loaded.
     */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta = ( editcondition = "bOverride_EraserMode" ) )
    bool       bEraserMode;

    /**
     * Modifier Advanced Blend Modes Override Value.
     *
     * If checked, Color and Alpha Blend Modes are set separately
     * when the brush is loaded.
     */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta = ( editcondition = "bOverride_IsComposite", DisplayName="Advanced Blend Modes" ) )
    bool       bIsComposite;

    /** Modifier Blending Mode Override Value. */
    UPROPERTY( EditAnywhere, Category="Modifiers",
        meta = (
            editcondition = "bOverride_BlendMode",
            InvalidEnumValues="Stencil"
        )
    )
    EOdysseyBlendMode       BlendMode;

    /** Modifier Color Blend Mode Override Value. */
    UPROPERTY( EditAnywhere, Category="Modifiers",
        meta = (
            DisplayName="Color Blend Mode",
            editcondition = "bOverride_CompositeColorBlendMode",
            InvalidEnumValues="Top,Back"
        )
    )
    EOdysseyColorBlendMode       CompositeColorBlendMode;

    /** Modifier Alpha Blend Mode Override Value. */
    UPROPERTY( EditAnywhere, Category="Modifiers",
        meta = (
            DisplayName="Alpha Blend Mode",
            editcondition = "bOverride_CompositeAlphaBlendMode",
            InvalidEnumValues="Min,Multiply,Top"
        )
    )
    EOdysseyAlphaBlendMode       CompositeAlphaBlendMode;
};
