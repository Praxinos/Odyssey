// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorGuide.h"

class FOdysseyHUDElement;

/**
 * Shows and edits one guide type in the painter editor viewport: its HUD shape and its handles.
 * Positions given to and returned by a visualizer are in texture pixels.
 */
class FOdysseyPainterEditorGuideVisualizer
{
public:
    virtual ~FOdysseyPainterEditorGuideVisualizer() = default;

    /** Returns the visualizer for the given guide type, or nullptr if the type has none. */
    static const FOdysseyPainterEditorGuideVisualizer* Find(EOdysseyPainterEditorGuideType InType);

    /** Creates the HUD element that draws a guide of this type. */
    virtual TSharedRef<FOdysseyHUDElement> CreateShape() const = 0;

    /** Updates a shape created by CreateShape so it matches the guide. */
    virtual void UpdateShape(FOdysseyHUDElement& InOutShape, const UOdysseyPainterEditorGuide& InGuide, const FVector2D& InTextureSize) const = 0;

    /** Fills the positions of the guide's handles, in handle index order. */
    virtual void GetHandlePositions(const UOdysseyPainterEditorGuide& InGuide, const FVector2D& InTextureSize, TArray<FVector2D>& OutPositions) const = 0;

    /** Moves the handle at InHandleIndex to InPosition and updates the guide accordingly. */
    virtual void SetHandlePosition(UOdysseyPainterEditorGuide& InOutGuide, int32 InHandleIndex, const FVector2D& InPosition, const FVector2D& InTextureSize) const = 0;
};
