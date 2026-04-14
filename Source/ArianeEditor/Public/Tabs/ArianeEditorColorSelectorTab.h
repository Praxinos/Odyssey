// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "ArianeEditorTab.h"

class FArianeEditor;
class UArianeEditorTool;

class ARIANEEDITOR_API FArianeEditorColorSelectorTab : public FArianeEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FArianeEditorColorSelectorTab();
    FArianeEditorColorSelectorTab( FArianeEditor* InEditor );

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

protected:
    // Widget Getters
    int WidgetIndex() const;

private:
    FArianeEditor* Editor;
};
