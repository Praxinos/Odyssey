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
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorHUDTab();
    FOdysseyPainterEditorHUDTab(FOdysseyPainterEditor* iEditor);

    void Init() override;

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    void RefreshWidgetContent();
    void OnSelectedToolChanged();
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

private:
    FOdysseyPainterEditor* mEditor;
};

