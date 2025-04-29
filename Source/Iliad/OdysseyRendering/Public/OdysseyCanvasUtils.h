// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImageAnchor.h"

class FCanvas;
class FBatchedElementParameters;

namespace Odyssey {
namespace CanvasUtils {

    ODYSSEYRENDERING_API void DrawTransformedQuad(FCanvas* iCanvas, FBatchedElementParameters* iShader, const FVector2D& iPosition, const FVector2D& iSourceSize, const FOdysseyImageAnchor& iHandlePosition, const FVector2D& iScale, float iRotationInDegrees, const FVector2D& iDestinationTextureSize);

} //CanvasUtils
} //Odyssey
