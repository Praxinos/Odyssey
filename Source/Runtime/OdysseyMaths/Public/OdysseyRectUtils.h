// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "CoreMinimal.h"
#include <ULIS>

namespace OdysseyRectUtils {
    //Retrieves the non overlapping rects representing the same area as the given rects
    ODYSSEYMATHS_API TArray< ::ULIS::FRectI > ToNonOverlappingRects(const TArray< ::ULIS::FRectI >& iRects);
}