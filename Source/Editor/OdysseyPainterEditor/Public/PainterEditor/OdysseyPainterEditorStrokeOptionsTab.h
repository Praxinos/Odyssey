// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorTab.h"

class FOdysseyPainterEditor;
class SOdysseyStrokeOptions;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorStrokeOptionsTab :
	public FOdysseyPainterEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorStrokeOptionsTab();
    FOdysseyPainterEditorStrokeOptionsTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyPainterEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters
    FOdysseyPaintEngine* PaintEngine() const;

protected:
    // Event Listeners

private:
    FOdysseyPainterEditor* mEditor;
};

