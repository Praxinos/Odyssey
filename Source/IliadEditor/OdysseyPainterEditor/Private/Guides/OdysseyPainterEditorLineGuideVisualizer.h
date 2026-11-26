// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "OdysseyPainterEditorGuideVisualizer.h"

/** Shows a line guide as a HUD line, with one handle per endpoint (0: start, 1: end). */
class FOdysseyPainterEditorLineGuideVisualizer : public FOdysseyPainterEditorGuideVisualizer
{
public:
    virtual TSharedRef<FOdysseyHUDElement> CreateShape() const override;
    virtual void UpdateShape(FOdysseyHUDElement& InOutShape, const UOdysseyPainterEditorGuide& InGuide, const FVector2D& InTextureSize) const override;
    virtual void GetHandlePositions(const UOdysseyPainterEditorGuide& InGuide, const FVector2D& InTextureSize, TArray<FVector2D>& OutPositions) const override;
    virtual void SetHandlePosition(UOdysseyPainterEditorGuide& InOutGuide, int32 InHandleIndex, const FVector2D& InPosition, const FVector2D& InTextureSize) const override;
};
