// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "OdysseyPainterEditorGuide.h"
#include "OdysseyPainterEditorEllipseGuide.generated.h"

/** Geometry of an ellipse guide at one instant, in normalized texture coordinates. */
USTRUCT()
struct ODYSSEYGUIDES_API FOdysseyPainterEditorEllipseGuideState
{
    GENERATED_BODY()

    /** Ellipse center in normalized texture coordinates, from (0, 0) to (1, 1). */
    UPROPERTY(EditAnywhere, Category="Ellipse")
    FVector2D Center = FVector2D(0.5f, 0.5f);

    /** Horizontal and vertical radii, each normalized to the corresponding texture dimension. */
    UPROPERTY(EditAnywhere, Category="Ellipse", meta=(ClampMin="0.0", UIMin="0.0"))
    FVector2D Radius = FVector2D(0.25f, 0.25f);

    /** Ellipse rotation in degrees. */
    UPROPERTY(EditAnywhere, Category="Ellipse")
    float Angle = 0.0f;
};

/** A visual guide represented by a rotated ellipse in normalized texture space. */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ODYSSEYGUIDES_API UOdysseyPainterEditorEllipseGuide : public UOdysseyPainterEditorGuide
{
    GENERATED_BODY()

public:
    /** Creates an ellipse guide with its default name. */
    UOdysseyPainterEditorEllipseGuide();
    /** Returns the ellipse guide type. */
    virtual EOdysseyPainterEditorGuideType GetGuideType() const override;

    /** Current ellipse geometry. */
    UPROPERTY(EditAnywhere, Category="Ellipse", meta=(ShowOnlyInnerProperties))
    FOdysseyPainterEditorEllipseGuideState State;
};
