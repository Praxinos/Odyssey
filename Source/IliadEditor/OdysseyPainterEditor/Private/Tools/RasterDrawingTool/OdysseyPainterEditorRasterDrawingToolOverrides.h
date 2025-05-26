// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"
#include "OdysseyPainterEditorRasterDrawingToolOverrides.generated.h"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorRasterDrawingToolOverrides
UCLASS(meta=(DisplayName="Raster Drawing Tool"))
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterDrawingToolOverrides : public UObject
{
    GENERATED_BODY()

public:
    UOdysseyPainterEditorRasterDrawingToolOverrides();

public:
    /////////////////////////////////////////////////////
    // Enable Overrides
    /** Enable Modifier SubPixel Override. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_Shape;

    UPROPERTY( EditAnywhere, Category="Modifiers", meta=(InlineEditConditionToggle) )
    bool    bOverride_SubPixel;


    /////////////////////////////////////////////////////
    // Overrides Values

    /** Modifier SubPixel Override Value. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta = ( editcondition = "bOverride_Shape" ) )
    EOdysseyShapeType Shape;

    /** Modifier SubPixel Override Value. */
    UPROPERTY( EditAnywhere, Category="Modifiers", meta = ( editcondition = "bOverride_SubPixel" ) )
    bool  SubPixel;
};
