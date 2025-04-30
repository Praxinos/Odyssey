// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyEditorTab.h"

class UOdysseyAnimation;
class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorAnimationDetailsTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorAnimationDetailsTab();
    FOdysseyPainterEditorAnimationDetailsTab(FOdysseyPainterEditor* iEditor);

protected:
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual bool CanOpen() const override;

protected:
    // Widget Getters
    virtual UOdysseyAnimation* Animation() const;

protected:
    // Event Listeners

private:
    FOdysseyPainterEditor* mEditor;
};
