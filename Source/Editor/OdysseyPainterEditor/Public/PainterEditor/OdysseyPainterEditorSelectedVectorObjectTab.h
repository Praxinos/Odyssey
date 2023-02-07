// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"
#include <ULIS>

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorSelectedVectorObjectTab :
    public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorSelectedVectorObjectTab();
    FOdysseyPainterEditorSelectedVectorObjectTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

protected:
    // Event Listeners
    void OnSelectionChanged();

protected:
    // Methods

private:
    FOdysseyPainterEditor* mEditor;
    TSharedPtr<IDetailsView> mDetailsView;
};

