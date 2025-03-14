// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include "OdysseyBrushContext.h"

//---

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorBrushContext
    : public FOdysseyBrushContext
{
public:
    virtual ~FOdysseyPainterEditorBrushContext();
    FOdysseyPainterEditorBrushContext(FOdysseyPainterEditor* iEditor);

public:
    FOdysseyPainterEditor*          Editor();

    float                           Zoom();
    float                           Rotation();
    FVector2D                       Pan();

    float                           GetStep();

private:
    FOdysseyPainterEditor*          mEditor;
};
