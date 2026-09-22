// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class UTexture2D;

namespace Odyssey {
#if WITH_EDITORONLY_DATA
    ODYSSEYCORE_API void ResizeTexture2D( UTexture2D* InOutTexture, uint32 InNewWidth, uint32 InNewHeight );
#endif
}
