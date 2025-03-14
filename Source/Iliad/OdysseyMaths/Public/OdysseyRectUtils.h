// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include <ULIS>
namespace OdysseyRectUtils {
    //Retrieves the non overlapping rects representing the same area as the given rects
    ODYSSEYMATHS_API TArray< ::ULIS::FRectI > ToNonOverlappingRects(const TArray< ::ULIS::FRectI >& iRects);

    //Returns a set of minimal rects occupying the same area as the input rects
    ODYSSEYMATHS_API TArray< ::ULIS::FRectI > MergeRects(const TArray< ::ULIS::FRectI >& iRects);
}
