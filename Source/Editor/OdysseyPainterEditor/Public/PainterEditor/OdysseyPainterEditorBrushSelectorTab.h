// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include <ULIS3>

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorBrushSelectorTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorBrushSelectorTab();
    FOdysseyPainterEditorBrushSelectorTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

protected:
    // Widget Getters
    virtual UOdysseyBrush* Brush() const;
    virtual UOdysseyBrushAssetBase* BrushInstance() const;

protected:
    // Event Listeners
    virtual void OnBrushSelected( UOdysseyBrush* iBrush );
    virtual void OnParameterChanged();

protected:
    // Methods
    virtual void RefreshBrush();

private:
    FOdysseyPainterEditor* mEditor;
};

