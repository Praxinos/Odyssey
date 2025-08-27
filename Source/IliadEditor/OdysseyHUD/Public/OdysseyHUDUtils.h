// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyHUDElement.h"

struct ODYSSEYHUD_API FOdysseyHUDUtils
{
    static void DrawCustomizedLine(const FVector2D& A, const FVector2D& B, const FOdysseyHUDElement::FHUDCustomization& iCustomization, const FOdysseyHUDElement::FDrawHUDParams& iParams, FBatchedElements* batchedElements);
};
