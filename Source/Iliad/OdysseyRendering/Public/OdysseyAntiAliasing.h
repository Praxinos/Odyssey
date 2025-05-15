// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include "RHI.h"

#include "OdysseyAntiAliasing.generated.h"

UENUM(BlueprintType)
enum class EOdysseyAntiAliasing : uint8
{
    NearestNeighbor,
    Bilinear,
    Trilinear,
    Anisotropic,
    AnisotropicLinear,
};

namespace Odyssey
{
    ODYSSEYRENDERING_API FSamplerStateRHIRef GetSamplerStateForAntiAliasing(EOdysseyAntiAliasing iAntiAliasing);
}
