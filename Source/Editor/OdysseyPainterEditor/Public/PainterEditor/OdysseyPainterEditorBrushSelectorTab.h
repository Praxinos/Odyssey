// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include <ULIS>

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorBrushSelectorTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorBrushSelectorTab();
    FOdysseyPainterEditorBrushSelectorTab(FOdysseyPainterEditor* iEditor);

public:
    /** Sets a widget to display when the given Tool is displayed */
    void SetWidgetForTool(class UOdysseyPainterEditorTool* iTool, TSharedPtr<SWidget> iWidget);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

protected:
    // Widget Getters
    int WidgetIndex() const;

private:
    FOdysseyPainterEditor* mEditor;

    struct FWidgetSlotForTool
    {
        class UOdysseyPainterEditorTool* mTool;
        SWidgetSwitcher::FSlot* mSlot;
    };
    TArray<FWidgetSlotForTool> mWidgetSlotForTool;
    TSharedPtr<SWidgetSwitcher> mWidgetSwitcher;
};

