// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyEditorTab.h"
#include "OdysseyEventState.h"
#include <ULIS>

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorColorSlidersTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorColorSlidersTab();
    FOdysseyPainterEditorColorSlidersTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
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
