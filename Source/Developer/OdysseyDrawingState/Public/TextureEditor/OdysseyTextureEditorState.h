// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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

