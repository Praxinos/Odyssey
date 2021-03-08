// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorGUI.h"

class FOdysseyPainterEditor;
class FOdysseyPainterEditorController;
class FOdysseyPainterEditorBrushExposedParametersTab;
class FOdysseyPainterEditorBrushSelectorTab;
class FOdysseyPainterEditorColorSlidersTab;
class FOdysseyPainterEditorColorWheelTab;
class FOdysseyPainterEditorMeshSelectorTab;
class FOdysseyPainterEditorStrokeOptionsTab;
class FOdysseyPainterEditorToolsTab;
class FOdysseyPainterEditorTopTab;
class FOdysseyPainterEditorViewportTab;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGUI :
	public FOdysseyEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorGUI();
    FOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor);

public:
    void Init();

public:
    // Tabs
    virtual void CreateTabs();
    virtual void BindShortcuts(FBaseToolkit* iToolkit);

public:
    // Menu And Toolbar
    virtual void FillExtender(FBaseToolkit* iToolkit, TSharedPtr<FExtender>& ioExtender);
    virtual void ExtendMenuAbout(FMenuBuilder& ioMenuBuilder);

protected:
	// Layout
	virtual TSharedPtr<FTabManager::FLayout> CreateLayout() override;
	virtual TSharedRef<FTabManager::FSplitter>	CreateMainSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateLeftSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateRightSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateMiddleSection();

public:
    // Getters
    TSharedPtr<FOdysseyPainterEditorViewportTab>& GetViewportTab();
    TSharedPtr<FOdysseyPainterEditorBrushSelectorTab>& GetBrushSelectorTab();
    TSharedPtr<FOdysseyPainterEditorMeshSelectorTab>& GetMeshSelectorTab();
    TSharedPtr<FOdysseyPainterEditorBrushExposedParametersTab>& GetBrushExposedParametersTab();
    TSharedPtr<FOdysseyPainterEditorColorWheelTab>& GetColorWheelTab();
    TSharedPtr<FOdysseyPainterEditorColorSlidersTab>& GetColorSlidersTab();
    TSharedPtr<FOdysseyPainterEditorStrokeOptionsTab>& GetStrokeOptionsTab();
    TSharedPtr<FOdysseyPainterEditorTopTab>& GetTopTab();
    TSharedPtr<FOdysseyPainterEditorToolsTab>& GetToolsTab();

protected:
    // Shortcuts
    virtual void VisitPraxinosWebsite();
    virtual void VisitPraxinosForums();
    virtual void AboutIliad();
    virtual void SwitchTabletAPI();

private:
    FOdysseyPainterEditor*                          mEditor;

protected:
    TSharedPtr<FOdysseyPainterEditorViewportTab>                mViewportTab;
    TSharedPtr<FOdysseyPainterEditorBrushSelectorTab>           mBrushSelectorTab;
    TSharedPtr<FOdysseyPainterEditorMeshSelectorTab>            mMeshSelectorTab;
    TSharedPtr<FOdysseyPainterEditorBrushExposedParametersTab>  mBrushExposedParametersTab;
    TSharedPtr<FOdysseyPainterEditorColorWheelTab>              mColorWheelTab;
    TSharedPtr<FOdysseyPainterEditorColorSlidersTab>            mColorSlidersTab;
    TSharedPtr<FOdysseyPainterEditorStrokeOptionsTab>           mStrokeOptionsTab;
    TSharedPtr<FOdysseyPainterEditorTopTab>                     mTopTab;
    TSharedPtr<FOdysseyPainterEditorToolsTab>                   mToolsTab;
};

