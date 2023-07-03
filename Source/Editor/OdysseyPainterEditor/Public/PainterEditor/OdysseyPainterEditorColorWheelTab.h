// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <ULIS>
#include "OdysseyEditorTab.h"
#include "OdysseyEventState.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorColorWheelTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorColorWheelTab();
    FOdysseyPainterEditorColorWheelTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters
    virtual ::ULIS::FColor Color() const;

protected:
    // Event Listeners
    virtual void OnColorChange( eOdysseyEventState::Type iEventState, const ::ULIS::FColor& iColor );

private:
    FOdysseyPainterEditor* mEditor;
};

