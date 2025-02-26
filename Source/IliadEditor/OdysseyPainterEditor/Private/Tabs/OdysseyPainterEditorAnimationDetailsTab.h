// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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

protected:
    // Widget Getters
    virtual UOdysseyAnimation* Animation() const;

protected:
    // Event Listeners

private:
    FOdysseyPainterEditor* mEditor;
};
