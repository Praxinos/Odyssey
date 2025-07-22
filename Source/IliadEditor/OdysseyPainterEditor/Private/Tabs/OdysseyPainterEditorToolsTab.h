// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyEditorTab.h"
#include <ULIS>

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorToolsTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorToolsTab();
    FOdysseyPainterEditorToolsTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

protected:
    // Event Listeners
    void OnToolSelected(UOdysseyPainterEditorTool* iTool);

protected:
    // Widget Getters
    int WidgetIndex() const;
    UOdysseyPainterEditorTool* GetCurrentTool() const;

private:
    FOdysseyPainterEditor* mEditor;
};
