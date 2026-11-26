// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "OdysseyPainterEditorGuideVisualizer.h"

/** Shows an ellipse guide as a HUD ellipse, with three handles (0: center, 1: X radius, 2: Y radius). */
class FOdysseyPainterEditorEllipseGuideVisualizer : public FOdysseyPainterEditorGuideVisualizer
{
public:
    virtual TSharedRef<FOdysseyHUDElement> CreateShape() const override;
    virtual void UpdateShape(FOdysseyHUDElement& InOutShape, const UOdysseyPainterEditorGuide& InGuide, const FVector2D& InTextureSize) const override;
    virtual void GetHandlePositions(const UOdysseyPainterEditorGuide& InGuide, const FVector2D& InTextureSize, TArray<FVector2D>& OutPositions) const override;
    virtual void SetHandlePosition(UOdysseyPainterEditorGuide& InOutGuide, int32 InHandleIndex, const FVector2D& InPosition, const FVector2D& InTextureSize) const override;

private:
    /** Unit vectors of the ellipse's X and Y axes for a rotation in degrees. */
    static void ComputeAxes(float InAngleDegrees, FVector2D& OutAxisX, FVector2D& OutAxisY);
};
