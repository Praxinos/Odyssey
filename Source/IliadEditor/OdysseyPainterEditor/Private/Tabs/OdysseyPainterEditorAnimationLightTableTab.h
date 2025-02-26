// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyEditorTab.h"

class FOdysseyPainterEditor;
class UOdysseyAnimationLayerStack;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorAnimationLightTableTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorAnimationLightTableTab();
    FOdysseyPainterEditorAnimationLightTableTab(FOdysseyPainterEditor* iEditor);

protected:
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters
    UOdysseyAnimationLayerStack* LayerStack() const;

protected:
    // Event

private:
    //Methods

private:
    FOdysseyPainterEditor* mEditor;
};
