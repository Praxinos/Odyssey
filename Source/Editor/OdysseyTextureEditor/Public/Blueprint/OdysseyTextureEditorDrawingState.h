// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyDrawingState.h"

class FOdysseyLayerStack;
class FOdysseyTextureEditor;

//---

class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorDrawingState
    : public FOdysseyDrawingState
{
public:
    static const FName& GetId();
    virtual const FName& Id() override;

public:
    FOdysseyTextureEditorDrawingState( FOdysseyTextureEditor* iEditor );

public:
    FOdysseyLayerStack* LayerStack();

private:
    FOdysseyTextureEditor*           mEditor;
};

