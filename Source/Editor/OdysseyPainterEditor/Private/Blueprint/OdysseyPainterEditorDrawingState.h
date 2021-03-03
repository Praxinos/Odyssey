// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyDrawingState.h"

//---

class FOdysseyPainterEditor;

class FOdysseyPainterEditorDrawingState
    : public FOdysseyDrawingState
{
public:
    static const FName& GetId();
    virtual const FName& Id() override;

public:
    virtual ~FOdysseyPainterEditorDrawingState();
    FOdysseyPainterEditorDrawingState(FOdysseyPainterEditor* iEditor);

public:
    float               Zoom();
    float               RotationInDegrees();
    FVector2D    Pan();

private:
    FOdysseyPainterEditor* mEditor;
};

