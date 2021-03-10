// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"
#include "OdysseyBrushAssetBase.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorBrushExposedParametersTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorBrushExposedParametersTab();
    FOdysseyPainterEditorBrushExposedParametersTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters
    virtual UOdysseyBrushAssetBase* BrushInstance() const;

protected:
    // Event Listeners
    virtual void OnParameterChanged();

private:
    FOdysseyPainterEditor* mEditor;
};

