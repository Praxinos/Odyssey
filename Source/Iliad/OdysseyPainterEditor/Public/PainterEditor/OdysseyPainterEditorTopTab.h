// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"
#include "Widgets/Layout/SWrapBox.h"

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorTopTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorTopTab();
    FOdysseyPainterEditorTopTab( FOdysseyPainterEditor* iEditor );

    void Init() override;

    // Spawner callback
    virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& iArgs) override;

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters

    virtual bool IsPackageEdited() const;

protected:
    // Event Listeners
    void UpdateToolWidget();
    void OnCurrentToolChanged();
    virtual FReply OnSaveButtonClicked();
    virtual FReply OnUndoButtonClicked();
    virtual FReply OnRedoButtonClicked();
    virtual FReply OnClearButtonClicked();

private:
    FOdysseyPainterEditor* mEditor;
    TSharedPtr<SWrapBox> mWrapBox;
    SWrapBox::FSlot* mToolWidgetSlot;
    UOdysseyPainterEditorTool* mCurrentTool;
};

