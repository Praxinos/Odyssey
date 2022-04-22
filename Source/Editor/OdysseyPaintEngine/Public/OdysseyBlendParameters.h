// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Image/OdysseyBlendingMode.h"
#include "OdysseyBlendParameters.generated.h"

USTRUCT(meta=(ShowOnlyInnerProperties))
struct ODYSSEYPAINTENGINE_API FOdysseyBlendParameters
{
    GENERATED_BODY()

    FOdysseyBlendParameters();

    FOdysseyBlendParameters(bool iEraserMode, EOdysseyBlendingMode iBlendingMode, EOdysseyAlphaMode iAlphaMode, float iOpacity);

    bool operator==(const FOdysseyBlendParameters& rhs) const;

    bool operator!=(const FOdysseyBlendParameters& rhs) const;

public:
    UPROPERTY(EditAnywhere)
    bool bEraserMode;

    UPROPERTY( EditAnywhere, meta = (EditCondition = "!bEraserMode"))
    EOdysseyBlendingMode BlendingMode;

    UPROPERTY( EditAnywhere, meta = (EditCondition = "!bEraserMode"))
    EOdysseyAlphaMode AlphaMode;

    UPROPERTY( EditAnywhere, meta = ( EditCondition="!bEraserMode", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple = "1", Units = "Percent"))
    float Opacity;
};