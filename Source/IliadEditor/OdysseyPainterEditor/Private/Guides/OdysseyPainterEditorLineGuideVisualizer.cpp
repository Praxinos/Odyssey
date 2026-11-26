// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorLineGuideVisualizer.h"

#include "OdysseyHUDLine.h"
#include "Shapes/OdysseyPainterEditorLineGuide.h"

TSharedRef<FOdysseyHUDElement> FOdysseyPainterEditorLineGuideVisualizer::CreateShape() const
{
    return MakeShared<FOdysseyHUDLine>(FVector2D::ZeroVector, FVector2D::ZeroVector);
}

void FOdysseyPainterEditorLineGuideVisualizer::UpdateShape(FOdysseyHUDElement& InOutShape, const UOdysseyPainterEditorGuide& InGuide, const FVector2D& InTextureSize) const
{
    const FOdysseyPainterEditorLineGuideState& State = CastChecked<UOdysseyPainterEditorLineGuide>(&InGuide)->State;
    FOdysseyHUDLine& Line = static_cast<FOdysseyHUDLine&>(InOutShape);
    Line.SetStartPoint(State.Start * InTextureSize);
    Line.SetEndPoint(State.End * InTextureSize);
    Line.SetColor(InGuide.Color);
    Line.SetOpacity(InGuide.Opacity);
    Line.SetThickness(InGuide.Thickness);
}

void FOdysseyPainterEditorLineGuideVisualizer::GetHandlePositions(const UOdysseyPainterEditorGuide& InGuide, const FVector2D& InTextureSize, TArray<FVector2D>& OutPositions) const
{
    const FOdysseyPainterEditorLineGuideState& State = CastChecked<UOdysseyPainterEditorLineGuide>(&InGuide)->State;
    OutPositions.Add(State.Start * InTextureSize);
    OutPositions.Add(State.End * InTextureSize);
}

void FOdysseyPainterEditorLineGuideVisualizer::SetHandlePosition(UOdysseyPainterEditorGuide& InOutGuide, int32 InHandleIndex, const FVector2D& InPosition, const FVector2D& InTextureSize) const
{
    FOdysseyPainterEditorLineGuideState& State = CastChecked<UOdysseyPainterEditorLineGuide>(&InOutGuide)->State;
    const FVector2D Position = (InPosition / InTextureSize).ClampAxes(0.0, 1.0);
    if (InHandleIndex == 0)
        State.Start = Position;
    else
        State.End = Position;
}
