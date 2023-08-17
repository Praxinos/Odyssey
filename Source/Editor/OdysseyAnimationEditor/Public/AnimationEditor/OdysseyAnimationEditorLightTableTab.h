// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorLightTableTab :
	public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorLightTableTab();
    FOdysseyAnimationEditorLightTableTab(FOdysseyAnimationEditorExtension* iEditor);

protected:
    // FOdysseyAnimationEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters

protected:
    // Event 

private:
    //Methods

private:
    FOdysseyAnimationEditorExtension* mExtension;
};

