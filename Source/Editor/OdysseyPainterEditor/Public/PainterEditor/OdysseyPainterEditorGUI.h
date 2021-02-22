// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

class FOdysseyPainterEditor;
class FOdysseyPainterEditorController;
class FOdysseyPainterEditorTab;
class FOdysseyPainterEditorBrushExposedParametersTab;
class FOdysseyPainterEditorBrushSelectorTab;
class FOdysseyPainterEditorColorSlidersTab;
class FOdysseyPainterEditorColorWheelTab;
class FOdysseyPainterEditorMeshSelectorTab;
class FOdysseyPainterEditorStrokeOptionsTab;
class FOdysseyPainterEditorToolsTab;
class FOdysseyPainterEditorTopTab;
class FOdysseyPainterEditorViewportTab;

#define ODYSSEY_ADD_TAB(var, class, ...) { var = MakeShareable(new class(__VA_ARGS__)); mTabs.Add(TAttribute<TSharedPtr<FOdysseyPainterEditorTab>>::Create([&](){ return var; })); }
#define ODYSSEY_SET_TAB(var, class, ...) { var = MakeShareable(new class(__VA_ARGS__)); }

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGUI :
	public TSharedFromThis<FOdysseyPainterEditorGUI>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorGUI();
    FOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor, const FName iLayoutName);

public:
    void Init();

public:
    // Tabs
    virtual void CreateTabs();
    virtual void InitTabs();
	virtual void RegisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef);
	virtual void UnregisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager);

public:
    // Menu And Toolbar
    virtual void FillExtender(TSharedPtr<FExtender>& ioExtender);
    virtual void ExtendMenuAbout(FMenuBuilder& ioMenuBuilder);

public:
    // Listeners
    virtual void OnToolkitInitialized();

protected:
	// Layout
	virtual void CreateLayout();
	virtual TSharedRef<FTabManager::FSplitter>	CreateMainSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateLeftSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateRightSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateMiddleSection();

public:
    // Getters
	TSharedRef<FTabManager::FLayout> GetLayout();

    TSharedPtr<FOdysseyPainterEditorViewportTab>& GetViewportTab();
    TSharedPtr<FOdysseyPainterEditorBrushSelectorTab>& GetBrushSelectorTab();
    TSharedPtr<FOdysseyPainterEditorMeshSelectorTab>& GetMeshSelectorTab();
    TSharedPtr<FOdysseyPainterEditorBrushExposedParametersTab>& GetBrushExposedParametersTab();
    TSharedPtr<FOdysseyPainterEditorColorWheelTab>& GetColorWheelTab();
    TSharedPtr<FOdysseyPainterEditorColorSlidersTab>& GetColorSlidersTab();
    TSharedPtr<FOdysseyPainterEditorStrokeOptionsTab>& GetStrokeOptionsTab();
    TSharedPtr<FOdysseyPainterEditorTopTab>& GetTopTab();
    TSharedPtr<FOdysseyPainterEditorToolsTab>& GetToolsTab();

private:
    FOdysseyPainterEditor*                          mEditor;
    FName                                           mLayoutName;
	TSharedPtr<FTabManager::FLayout>                mLayout;

protected:
    //listing all tabs and managing the fact that a tab ptr can change through the Init process
    TArray<TAttribute<TSharedPtr<FOdysseyPainterEditorTab>>> mTabs;

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

