// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyDrawingState.h"

//---

class ODYSSEYDRAWINGSTATE_API FOdysseyPainterEditorState
    : public FOdysseyDrawingState
{
public:
    static const FName& GetId();

public:
    FOdysseyPainterEditorState( float iZoom, float iRotationInDegrees, const FVector2D& iPan );

public:
    float               Zoom();
    float               RotationInDegrees();
    const FVector2D&    Pan();

private:
    float       mZoom;
    float       mRotation;
    FVector2D   mPan;
};

