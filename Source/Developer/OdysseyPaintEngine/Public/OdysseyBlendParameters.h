// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Image/OdysseyBlendingMode.h"
#include "OdysseyBlendParameters.generated.h"

USTRUCT()
struct ODYSSEYPAINTENGINE_API FOdysseyBlendParameters
{
    GENERATED_BODY()

    FOdysseyBlendParameters()
        : bEraserMode(false)
        , BlendingMode(EOdysseyBlendingMode::kNormal)
        , AlphaMode(EOdysseyAlphaMode::kNormal)
        , Opacity(100.0f)
    {
    }

    FOdysseyBlendParameters(bool iEraserMode, EOdysseyBlendingMode iBlendingMode, EOdysseyAlphaMode iAlphaMode, float iOpacity)
        : bEraserMode(iEraserMode)
        , BlendingMode(iBlendingMode)
        , AlphaMode(iAlphaMode)
        , Opacity(iOpacity)
    {
    }

    bool operator==(const FOdysseyBlendParameters& rhs) const
    {
        return BlendingMode == rhs.BlendingMode && AlphaMode == rhs.AlphaMode && Opacity == rhs.Opacity && bEraserMode == rhs.bEraserMode; // or another approach as above
    }

    bool operator!=(const FOdysseyBlendParameters& rhs) const
    {
        return !operator==(rhs);
    }

public:
    UPROPERTY(EditAnywhere, Category = "PaintEngine")
    bool bEraserMode;

    UPROPERTY( EditAnywhere, Category = "PaintEngine", meta = (EditCondition = "!bEraserMode"))
    EOdysseyBlendingMode BlendingMode;

    UPROPERTY( EditAnywhere, Category = "PaintEngine", meta = (EditCondition = "!bEraserMode"))
    EOdysseyAlphaMode AlphaMode;

    UPROPERTY( EditAnywhere, Category = "PaintEngine", meta = ( EditCondition="!bEraserMode", ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple = "1", Units = "Percent"))
    float Opacity;
};