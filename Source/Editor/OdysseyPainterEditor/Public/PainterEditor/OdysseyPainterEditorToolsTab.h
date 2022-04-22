// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"
#include <ULIS>

class FOdysseyPainterEditor;
class UOdysseyTool;


enum eToolType
{
    kTool_FreeHand,
    kTool_Line,
    kTool_Rectangle,
    kTool_Polygon,
    kTool_Circle,
    kTool_Ellipse,
    kTool_Bezier
};

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorToolsTab :
    public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorToolsTab();
    FOdysseyPainterEditorToolsTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

protected:
    // Event Listeners
    virtual FReply OnClear();
    virtual FReply OnFill();
    virtual FReply OnClearUndo();

    /** Delegate for Tools checkBoxes button */
    void OnToolCheckBoxClicked(ECheckBoxState iCheckBoxState, eToolType iToolType);

protected:
    // Methods
    virtual void Clear();
    virtual void Fill();

private:
    FOdysseyPainterEditor* mEditor;
};

