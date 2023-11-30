// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyBrushContext.h"

//---

class UOdysseyLayerStack;

class ODYSSEYLAYERSTACKEDITOR_API FOdysseyLayerStackEditorBrushContext
    : public FOdysseyBrushContext
{
public:
    virtual ~FOdysseyLayerStackEditorBrushContext();
    FOdysseyLayerStackEditorBrushContext(UOdysseyLayerStack* iLayerStack);

public:
    UOdysseyLayerStack*          GetLayerStack() const;
    void          SetLayerStack(UOdysseyLayerStack* iLayerStack);
private:
    UOdysseyLayerStack*          mLayerStack;
};

