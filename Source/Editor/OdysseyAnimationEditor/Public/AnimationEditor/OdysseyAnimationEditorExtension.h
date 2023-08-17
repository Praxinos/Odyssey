// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "PainterEditor/OdysseyPainterEditorExtension.h"

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorExtension
    : public FOdysseyPainterEditorExtension
{   
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorExtension();
    FOdysseyAnimationEditorExtension( FOdysseyPainterEditor* iEditor );

public:
    virtual void Install() override;
    virtual void Uninstall() override;

public:
};
