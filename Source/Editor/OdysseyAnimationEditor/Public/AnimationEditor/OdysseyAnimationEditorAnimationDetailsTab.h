// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"

class UOdysseyAnimation;
class FOdysseyAnimationEditorExtension;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorAnimationDetailsTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorAnimationDetailsTab();
    FOdysseyAnimationEditorAnimationDetailsTab(FOdysseyAnimationEditorExtension* iExtension);

protected:
    // FOdysseyAnimationEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters
    virtual UOdysseyAnimation* Animation() const;

protected:
    // Event Listeners

private:
    FOdysseyAnimationEditorExtension* mExtension;
};

