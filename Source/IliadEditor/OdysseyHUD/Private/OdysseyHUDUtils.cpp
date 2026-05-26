// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDUtils.h"

#include "BatchedElements.h"
#include "CanvasTypes.h"

void FOdysseyHUDUtils::DrawCustomizedLine(const FVector2D& iA, const FVector2D& iB, const FOdysseyHUDElement::FHUDCustomization& iCustomization, const FOdysseyHUDElement::FDrawHUDParams& iParams, FBatchedElements* iBatchedElements)
{
    if( !iBatchedElements )
        return;

    TArray<FLinearColor> colors = iCustomization.mColors;
    if (colors.Num() == 0)
        colors.Add(FLinearColor::Black);

    if(iCustomization.mSegmentLength <= 0.f || iCustomization.mGapLength < 0.f )
        return;

    FVector2D dir = iB - iA;
    float totalLength = dir.Size();
    dir.Normalize();

    float current = 0.f;

    int colorIndex = 0;
    int numColors = colors.Num();
    while (current < totalLength)
    {
        float next = FMath::Min(current + iCustomization.mSegmentLength, totalLength);

        FVector2D start = iA + dir * current;
        FVector2D end = iA + dir * next;

        iBatchedElements->AddTranslucentLine(
            FVector(start, 0.f),
            FVector(end, 0.f),
            colors[colorIndex],
            iParams.mCanvas->GetHitProxyId(),
            1.f,   // thickness
            0.f,   // depth bias
            true   // antialiasing
        );

        current = next + iCustomization.mGapLength;
        colorIndex = (colorIndex + 1) % numColors;
    }
}
