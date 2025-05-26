// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyEditorShortcuts.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGlobalShortcuts
    : public FOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyPainterEditorGlobalShortcuts() {};
    FOdysseyPainterEditorGlobalShortcuts(FOdysseyPainterEditor* iEditor);

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

public:
    void Action_CopyCurrentSelection();
    void Action_CutCurrentSelection();
    void Action_PasteCurrentSelection();
    void Action_PasteCurrentSelectionInNewLayer();

    bool CanAction_CopyCurrentSelection();
    bool CanAction_CutCurrentSelection();
    bool CanAction_PasteCurrentSelection();
    bool CanAction_PasteCurrentSelectionInNewLayer();

private:
    FOdysseyPainterEditor* mEditor;
};
