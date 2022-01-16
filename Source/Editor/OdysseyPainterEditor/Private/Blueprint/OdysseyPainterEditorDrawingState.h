// IDDN FR.001.250001.005.S.P.2019.000.00000
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
    float               Rotation();
    FVector2D    Pan();

private:
    FOdysseyPainterEditor* mEditor;
};

