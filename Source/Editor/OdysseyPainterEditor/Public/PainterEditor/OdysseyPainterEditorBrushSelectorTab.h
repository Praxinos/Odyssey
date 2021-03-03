// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorTab.h"
#include <ULIS3>

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorBrushSelectorTab :
	public FOdysseyPainterEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorBrushSelectorTab();
    FOdysseyPainterEditorBrushSelectorTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyPainterEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual TSharedRef< SDockTab > SpawnTab( const FSpawnTabArgs& iArgs ) override;
    virtual void BindShortcuts() override;

protected:
    // Widget Getters
    virtual UOdysseyBrush* Brush() const;

protected:
    // Event Listeners
    virtual void OnBrushSelected( UOdysseyBrush* iBrush );

protected:
    // Methods
    virtual void RefreshBrush();

private:
    FOdysseyPainterEditor* mEditor;
};

