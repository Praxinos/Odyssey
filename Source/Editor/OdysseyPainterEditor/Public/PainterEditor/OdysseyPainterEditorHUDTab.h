// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"
#include "OdysseyEventState.h"
#include "OdysseyHUDElement.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorHUDTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorHUDTab();
    FOdysseyPainterEditorHUDTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

public:
    // Public Getters
    TSharedPtr<UOdysseyHUDElement> GetHUD();

private:
    FOdysseyPainterEditor* mEditor;

    TSharedPtr<UOdysseyHUDElement> mHUD;
};

