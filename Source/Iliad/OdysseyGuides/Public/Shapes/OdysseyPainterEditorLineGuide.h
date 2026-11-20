// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "OdysseyPainterEditorGuide.h"
#include "OdysseyPainterEditorLineGuide.generated.h"

/** A visual guide represented by a line between two normalized texture positions. */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ODYSSEYGUIDES_API UOdysseyPainterEditorLineGuide : public UOdysseyPainterEditorGuide
{
    GENERATED_BODY()

public:
    /** Creates a line guide with its default name. */
    UOdysseyPainterEditorLineGuide();
    /** Returns the line guide type. */
    virtual EOdysseyPainterEditorGuideType GetGuideType() const override;

    /** Line start in normalized texture coordinates, from (0, 0) to (1, 1). */
    UPROPERTY(EditAnywhere, Category="Line")
    FVector2D Start = FVector2D(0.25f, 0.5f);

    /** Line end in normalized texture coordinates, from (0, 0) to (1, 1). */
    UPROPERTY(EditAnywhere, Category="Line")
    FVector2D End = FVector2D(0.75f, 0.5f);
};
