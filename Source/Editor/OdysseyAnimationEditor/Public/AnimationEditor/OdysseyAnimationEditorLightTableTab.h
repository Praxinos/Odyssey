// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorLightTableTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorLightTableTab();
    FOdysseyAnimationEditorLightTableTab(FOdysseyAnimationEditor* iEditor);

protected:
    // FOdysseyAnimationEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters

protected:
    // Event 

private:
    //Methods

private:
    FOdysseyAnimationEditor* mEditor;
};

