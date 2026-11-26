// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorEllipseGuideVisualizer.h"

#include "OdysseyHUDEllipse.h"
#include "Shapes/OdysseyPainterEditorEllipseGuide.h"

TSharedRef<FOdysseyHUDElement> FOdysseyPainterEditorEllipseGuideVisualizer::CreateShape() const
{
    return MakeShared<FOdysseyHUDEllipse>(FVector2D::ZeroVector, FVector2D::ZeroVector);
}

void FOdysseyPainterEditorEllipseGuideVisualizer::UpdateShape(FOdysseyHUDElement& InOutShape, const UOdysseyPainterEditorGuide& InGuide, const FVector2D& InTextureSize) const
{
    const FOdysseyPainterEditorEllipseGuideState& State = CastChecked<UOdysseyPainterEditorEllipseGuide>(&InGuide)->State;
    FOdysseyHUDEllipse& Ellipse = static_cast<FOdysseyHUDEllipse&>(InOutShape);
    Ellipse.SetCenter(State.Center * InTextureSize);
    Ellipse.SetRadius(State.Radius * InTextureSize);
    Ellipse.SetRotation(State.Angle);
    Ellipse.SetColor(InGuide.Color);
    Ellipse.SetOpacity(InGuide.Opacity);
    Ellipse.SetThickness(InGuide.Thickness);
}

void FOdysseyPainterEditorEllipseGuideVisualizer::GetHandlePositions(const UOdysseyPainterEditorGuide& InGuide, const FVector2D& InTextureSize, TArray<FVector2D>& OutPositions) const
{
    const FOdysseyPainterEditorEllipseGuideState& State = CastChecked<UOdysseyPainterEditorEllipseGuide>(&InGuide)->State;
    FVector2D AxisX;
    FVector2D AxisY;
    ComputeAxes(State.Angle, AxisX, AxisY);

    const FVector2D Center = State.Center * InTextureSize;
    OutPositions.Add(Center);
    OutPositions.Add(Center + AxisX * (State.Radius.X * InTextureSize.X));
    OutPositions.Add(Center + AxisY * (State.Radius.Y * InTextureSize.Y));
}

void FOdysseyPainterEditorEllipseGuideVisualizer::SetHandlePosition(UOdysseyPainterEditorGuide& InOutGuide, int32 InHandleIndex, const FVector2D& InPosition, const FVector2D& InTextureSize) const
{
    FOdysseyPainterEditorEllipseGuideState& State = CastChecked<UOdysseyPainterEditorEllipseGuide>(&InOutGuide)->State;
    const FVector2D Position = (InPosition / InTextureSize).ClampAxes(0.0, 1.0);
    if (InHandleIndex == 0)
    {
        State.Center = Position;
        return;
    }

    // Each radius handle only changes its own radius, measured along its rotated axis (in pixels, so
    // the rotation stays correct on non-square textures).
    FVector2D AxisX;
    FVector2D AxisY;
    ComputeAxes(State.Angle, AxisX, AxisY);
    const FVector2D DeltaPixels = (Position - State.Center) * InTextureSize;
    if (InHandleIndex == 1)
        State.Radius.X = FMath::Clamp(FMath::Abs(FVector2D::DotProduct(DeltaPixels, AxisX)) / InTextureSize.X, 0.0, 1.0);
    else
        State.Radius.Y = FMath::Clamp(FMath::Abs(FVector2D::DotProduct(DeltaPixels, AxisY)) / InTextureSize.Y, 0.0, 1.0);
}

void FOdysseyPainterEditorEllipseGuideVisualizer::ComputeAxes(float InAngleDegrees, FVector2D& OutAxisX, FVector2D& OutAxisY)
{
    const double AngleRadians = FMath::DegreesToRadians(InAngleDegrees);
    OutAxisX = FVector2D(FMath::Cos(AngleRadians), FMath::Sin(AngleRadians));
    OutAxisY = FVector2D(-OutAxisX.Y, OutAxisX.X);
}
