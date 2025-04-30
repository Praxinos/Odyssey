// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyEditorTab.h"

class FOdysseyPainterEditor;
class UOdysseyAnimationLayerStack;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorAnimationLighttableTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorAnimationLighttableTab();
    FOdysseyPainterEditorAnimationLighttableTab(FOdysseyPainterEditor* iEditor);

protected:
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual bool CanOpen() const override;

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
