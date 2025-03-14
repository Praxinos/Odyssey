// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorShortcuts.h"

class FUICommandList;
class FOdysseyPainterEditor;
class ODYSSEYPAINTEREDITOR_API FOdysseyAnimationGlobalShortcuts
    : public FOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyAnimationGlobalShortcuts() {};
    FOdysseyAnimationGlobalShortcuts(FOdysseyPainterEditor* iEditor);

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    FOdysseyPainterEditor* mEditor;
};
