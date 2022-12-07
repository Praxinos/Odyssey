// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include <ULIS>

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorToolOptionsTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorToolOptionsTab();
    FOdysseyPainterEditorToolOptionsTab(FOdysseyPainterEditor* iEditor);

public:
    /** Sets a widget to display when the given Tool is displayed */
    void SetWidgetForTool(UOdysseyTool* iTool, TSharedPtr<SWidget> iWidget);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

protected:
    // Widget Getters
    int WidgetIndex() const;

protected:
    // Event Listeners

protected:
    // Methods

private:
    FOdysseyPainterEditor* mEditor;

    struct FWidgetSlotForTool
    {
        UOdysseyTool* mTool;
        SWidgetSwitcher::FSlot* mSlot;
    };
    TArray<FWidgetSlotForTool> mWidgetSlotForTool;
    TSharedPtr<SWidgetSwitcher> mWidgetSwitcher;
};

