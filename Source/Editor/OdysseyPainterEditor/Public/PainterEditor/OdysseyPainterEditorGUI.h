// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

// class SOdysseyBrushExposedParameters;
// class SOdysseyBrushSelector;
class SOdysseyLayerStackView;
class SOdysseyPaintModifiers;
// class SOdysseyStrokeOptions;
class SOdysseyUndoHistory;

class SDockableTab;
class STextBlock;

class FOdysseyPainterEditor;
class FOdysseyPainterEditorController;
class FOdysseyPainterEditorTab;
class FOdysseyPainterEditorBrushExposedParametersTab;
class FOdysseyPainterEditorBrushSelectorTab;
class FOdysseyPainterEditorColorSlidersTab;
class FOdysseyPainterEditorColorWheelTab;
class FOdysseyPainterEditorMeshSelectorTab;
class FOdysseyPainterEditorStrokeOptionsTab;
class FOdysseyPainterEditorTopTab;
class FOdysseyPainterEditorViewportTab;

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
    void OnToolkitInitialized();

protected:
    virtual void CreateTabs();
    virtual void InitTabs();

private:
    // void CreateViewportTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController);
    // void CreateBrushSelectorTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController);
    // void CreateMeshSelectorTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController);
    // void CreateBrushExposedParametersTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController);
    // void CreateColorSelectorTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController);
    // void CreateColorSlidersTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController);
    // void CreateTopTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController);
    void CreateToolsTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController);
    // void CreateStrokeOptionsTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController);
    //void CreateUndoHistoryTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController);

protected:
	virtual void RegisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef);
	virtual void UnregisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager);

private:
	// Spawner callbacks
	// Callback for spawning the Brush Selector tab.
	// TSharedRef<SDockTab> HandleTabSpawnerSpawnBrushSelector(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Mesh Selector tab.
	// TSharedRef<SDockTab> HandleTabSpawnerSpawnMeshSelector(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Brush Parameters tab.
	// TSharedRef<SDockTab> HandleTabSpawnerSpawnBrushExposedParameters(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Viewport tab.
	// TSharedRef<SDockTab> HandleTabSpawnerSpawnViewport(const FSpawnTabArgs& iArgs);
	// Callback for spawning the ColorSelector tab.
	// TSharedRef<SDockTab> HandleTabSpawnerSpawnColorSelector(const FSpawnTabArgs& iArgs);
	// Callback for spawning the ColorSelector tab.
	// TSharedRef<SDockTab> HandleTabSpawnerSpawnColorSliders(const FSpawnTabArgs& iArgs);
	// Callback for spawning the LayerStack tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& iArgs);
	// Callback for spawning the TopBar tab.
	// TSharedRef<SDockTab> HandleTabSpawnerSpawnTopBar(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Smoothing Options tab.
	// TSharedRef<SDockTab> HandleTabSpawnerSpawnStrokeOptions(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Smoothing Options tab.
	// Callback for spawning the Notes tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnNotes(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Undo History tab.
	//TSharedRef<SDockTab> HandleTabSpawnerSpawnUndoHistory(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Tools tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnTools(const FSpawnTabArgs& iArgs);

protected:
	virtual void InitOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController> iController);

	// Internal widget creation
	virtual void CreateLayout();
	virtual void CreateWidgets(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController);
	virtual TSharedRef<FTabManager::FSplitter>	CreateMainSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateLeftSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateRightSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateMiddleSection();

public:
	TSharedRef<FTabManager::FLayout> GetLayout();

    TSharedPtr<FOdysseyPainterEditorViewportTab>& GetViewportTab();
    TSharedPtr<FOdysseyPainterEditorBrushSelectorTab>& GetBrushSelectorTab();
    TSharedPtr<FOdysseyPainterEditorMeshSelectorTab>& GetMeshSelectorTab();
    TSharedPtr<FOdysseyPainterEditorBrushExposedParametersTab>& GetBrushExposedParametersTab();
    TSharedPtr<FOdysseyPainterEditorColorWheelTab>& GetColorWheelTab();
    TSharedPtr<FOdysseyPainterEditorColorSlidersTab>& GetColorSlidersTab();
    TSharedPtr<FOdysseyPainterEditorStrokeOptionsTab>& GetStrokeOptionsTab();
    //TSharedPtr<SOdysseyUndoHistory>& GetUndoHistoryTab();
    TSharedPtr<FOdysseyPainterEditorTopTab>& GetTopTab();
    TSharedPtr<SWidget>& GetToolsTab();

private:
    FOdysseyPainterEditor*                          mEditor;
    FName                                           mLayoutName;
	TSharedPtr<FTabManager::FLayout>                mLayout;

protected:
    TSharedPtr<FOdysseyPainterEditorViewportTab>                mViewportTab;
    TSharedPtr<FOdysseyPainterEditorBrushSelectorTab>                           mBrushSelectorTab;
    TSharedPtr<FOdysseyPainterEditorMeshSelectorTab>            mMeshSelectorTab;
    TSharedPtr<FOdysseyPainterEditorBrushExposedParametersTab>  mBrushExposedParametersTab;
    TSharedPtr<FOdysseyPainterEditorColorWheelTab>              mColorWheelTab;
    TSharedPtr<FOdysseyPainterEditorColorSlidersTab>            mColorSlidersTab;
    TSharedPtr<SOdysseyLayerStackView>                          mLayerStackTab;
    TSharedPtr<FOdysseyPainterEditorStrokeOptionsTab>           mStrokeOptionsTab;
    //TSharedPtr<SOdysseyUndoHistory>                           mUndoHistoryTab;
    TSharedPtr<FOdysseyPainterEditorTopTab>                     mTopTab;
    TSharedPtr<SWidget>                                         mToolsTab;

private:
    /** Tabs IDs */
    // static const FName smViewportTabId;
    // static const FName smBrushSelectorTabId;
    // static const FName smMeshSelectorTabId;
    // static const FName smBrushExposedParametersTabId;
    // static const FName smColorSelectorTabId;
    // static const FName smColorSlidersTabId;
    static const FName smLayerStackTabId;
    // static const FName smTopBarTabId;
    // static const FName smStrokeOptionsTabId;
    static const FName smNotesTabId;
    static const FName smUndoHistoryTabId;
    static const FName smToolsTabId;
};

