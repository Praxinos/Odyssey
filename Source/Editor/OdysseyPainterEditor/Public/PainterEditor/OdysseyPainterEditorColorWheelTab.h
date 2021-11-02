// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"

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

