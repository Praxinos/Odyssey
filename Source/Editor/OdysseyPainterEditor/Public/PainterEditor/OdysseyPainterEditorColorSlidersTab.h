// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"
#include "OdysseyEventState.h"
#include <ULIS3>

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorColorSlidersTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorColorSlidersTab();
    FOdysseyPainterEditorColorSlidersTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
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

