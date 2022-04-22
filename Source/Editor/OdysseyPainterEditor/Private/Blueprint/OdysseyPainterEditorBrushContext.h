// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyBrushContext.h"

//---

class FOdysseyPainterEditor;

class FOdysseyPainterEditorBrushContext
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

