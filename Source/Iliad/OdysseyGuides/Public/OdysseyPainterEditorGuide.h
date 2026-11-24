// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorGuide.generated.h"

/** Identifies a guide shape that can be stored in an Odyssey guide collection. */
UENUM()
enum class EOdysseyPainterEditorGuideType : uint8
{
    /** A straight segment with independently editable start and end points. */
    Line,
    /** An ellipse with an editable center, radii, and rotation. */
    Ellipse,
    /** Default value for UOdysseyPainterEditorGuide. */
    Undefined
};

/** Base data for a visual guide displayed over an Odyssey texture or animation. */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class ODYSSEYGUIDES_API UOdysseyPainterEditorGuide : public UObject
{
    GENERATED_BODY()

public:
    /** Guides are editor-only and never cooked. */
    virtual bool IsEditorOnly() const override { return true; }

    /** Returns the shape type represented by this guide. */
    virtual EOdysseyPainterEditorGuideType GetGuideType() const;

    /** User-editable name shown in the Guides panel. */
    UPROPERTY(EditAnywhere, Category="Guide")
    FText Name;

    /** Whether this guide is drawn in the editor viewport. */
    UPROPERTY(EditAnywhere, Category="Guide")
    bool bVisible = true;

    /** Whether viewport handle editing is disabled for this guide. */
    UPROPERTY(EditAnywhere, Category="Guide")
    bool bLocked = false;

    /** Guide stroke color. */
    UPROPERTY(EditAnywhere, Category="Guide")
    FLinearColor Color = FLinearColor(0.0f, 0.25f, 1.0f, 1.0f);

    /** Guide stroke opacity, from 0 (transparent) to 1 (opaque). */
    UPROPERTY(EditAnywhere, Category="Guide", meta=(ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0"))
    float Opacity = 1.0f;

    /** Guide stroke width in viewport units. */
    UPROPERTY(EditAnywhere, Category="Guide", meta=(ClampMin="0.5", UIMin="0.5", ClampMax="10.0", UIMax="10.0"))
    float Thickness = 2.0f;
};
