// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorTab.h"
#include <ULIS3>

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorColorSlidersTab :
	public FOdysseyPainterEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorColorSlidersTab();
    FOdysseyPainterEditorColorSlidersTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyPainterEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters
    virtual ::ul3::FPixelValue Color() const;

protected:
    // Event Listeners
    virtual void OnColorChange( eOdysseyEventState::Type iEventState, const ::ul3::FPixelValue& iColor );

private:
    FOdysseyPainterEditor* mEditor;
};

