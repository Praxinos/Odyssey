// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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

private:
    FOdysseyPainterEditor*          mEditor;
};

