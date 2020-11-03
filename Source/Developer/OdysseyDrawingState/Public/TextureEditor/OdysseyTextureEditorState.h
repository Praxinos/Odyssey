// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyDrawingState.h"

class FOdysseyLayerStack;

//---

class ODYSSEYDRAWINGSTATE_API FOdysseyTextureEditorState
    : public FOdysseyDrawingState
{
public:
    static const FName& GetId();

public:
    FOdysseyTextureEditorState( FOdysseyLayerStack* iLayerStack );

public:
    FOdysseyLayerStack* LayerStack();

private:
    FOdysseyLayerStack*           mLayerStack;
};

