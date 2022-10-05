// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyBrushAssetBase.h"
#include "UObject/ObjectMacros.h"
#include "Image/OdysseyBlendingMode.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyBlendParametersOverrides.generated.h"

/////////////////////////////////////////////////////
// FOdysseyBlendParametersOverrides
UCLASS(meta=(DisplayName="Blend Parameters"))
class ODYSSEYPAINTEREDITOR_API UOdysseyBlendParametersOverrides : public UObject
{
    GENERATED_BODY()

    UOdysseyBlendParametersOverrides();

    /** Enable Modifier Opacity Override. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_Opacity;

    /** Enable Modifier BlendingMode Override. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_BlendingMode;

    /** Enable Modifier AlphaMode Override. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_AlphaMode;

    /////////////////////////////////////////////////////
    // Overrides Values
    /** Modifier Opacity Override Value. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", SliderExponent = "1", editcondition = "bOverride_Opacity" ) )
    float  Opacity;

    /** Modifier BlendingMode Override Value. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta = ( editcondition = "bOverride_BlendingMode" ) )
    EOdysseyBlendingMode    BlendingMode;

    /** Modifier AlphaMode Override Value. */
    UPROPERTY( EditAnywhere, Category = "Modifiers", meta = ( editcondition = "bOverride_AlphaMode" ) )
    EOdysseyAlphaMode       AlphaMode;

public:
    //Applies the Overrides to the given object
    void Override(FOdysseyBlendParameters& iBlendParameters) const;
};