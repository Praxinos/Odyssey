// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"

#include "ArianeLayerVectorEnums.generated.h"

UENUM()
enum class EArianeLayerVectorDrawingOrigin : uint8
{
    Layer,
    //View,
    Surface,
};

UENUM()
enum class EArianeLayerVectorDrawingOrientation : uint8
{
    XY,
    YZ,
    ZX,
    View,
};
