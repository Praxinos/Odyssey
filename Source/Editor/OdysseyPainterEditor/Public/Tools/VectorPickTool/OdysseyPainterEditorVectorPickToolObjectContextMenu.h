// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditorContextMenu.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPickToolObjectContextMenu :
    public FOdysseyPainterEditorContextMenu
{
public:
    // FOdysseyEditorTab interface
    static TSharedPtr<SWidget> CreateWidget( class FOdysseyPainterEditor* iPainterEditor );
};
