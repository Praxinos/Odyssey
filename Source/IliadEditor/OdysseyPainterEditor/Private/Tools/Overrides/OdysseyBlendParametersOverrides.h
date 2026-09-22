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

    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_IsComposite;

    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_BlendMode;

    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_CompositeColorBlendMode;

    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_CompositeAlphaBlendMode;

    /////////////////////////////////////////////////////
    // Overrides Values
    /** Modifier Opacity Override Value. */
    UPROPERTY()
    float  Opacity;

    UPROPERTY( EditAnywhere, Category="Modifiers", meta = ( editcondition = "bOverride_IsComposite" ) )
    bool       bIsComposite;

    UPROPERTY( EditAnywhere, Category="Modifiers",
        meta = (
            editcondition = "bOverride_BlendMode",
            InvalidEnumValues="Stencil"
        )
    )
    EOdysseyBlendMode       BlendMode;

    UPROPERTY( EditAnywhere, Category="Modifiers",
        meta = (
            DisplayName="Color Blend Mode",
            editcondition = "bOverride_CompositeColorBlendMode",
            InvalidEnumValues="Top,Back"
        )
    )
    EOdysseyColorBlendMode       CompositeColorBlendMode;

    UPROPERTY( EditAnywhere, Category="Modifiers",
        meta = (
            DisplayName="Alpha Blend Mode",
            editcondition = "bOverride_CompositeAlphaBlendMode",
            InvalidEnumValues="Min,Multiply,Top"
        )
    )
    EOdysseyAlphaBlendMode       CompositeAlphaBlendMode;
};
