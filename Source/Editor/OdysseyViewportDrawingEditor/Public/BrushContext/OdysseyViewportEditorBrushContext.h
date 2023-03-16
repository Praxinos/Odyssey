// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyBrushContext.h"

//---

class FOdysseyPainterEditor;

class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportEditorBrushContext
    : public FOdysseyBrushContext
{
public:
    virtual ~FOdysseyViewportEditorBrushContext();
    FOdysseyViewportEditorBrushContext();

public:
    float                           GetStep();
    float                           GetSize();
};

