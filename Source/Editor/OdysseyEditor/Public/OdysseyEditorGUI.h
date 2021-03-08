// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

class FOdysseyEditor;
class FOdysseyEditorTab;

#define ODYSSEY_ADD_TAB(var, class, ...) { var = MakeShareable(new class(__VA_ARGS__)); mTabs.Add(TAttribute<TSharedPtr<FOdysseyEditorTab>>::Create([&](){ return var; })); }
#define ODYSSEY_SET_TAB(var, class, ...) { var = MakeShareable(new class(__VA_ARGS__)); }

/**
 * Implements an Editor GUI.
 */
class ODYSSEYEDITOR_API FOdysseyEditorGUI :
	public TSharedFromThis<FOdysseyEditorGUI>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyEditorGUI();
    FOdysseyEditorGUI(FOdysseyEditor* iEditor);

public:
    void Init();

public:
    // Tabs
    virtual void CreateTabs();
    virtual void InitTabs();
    virtual void BindShortcuts(FBaseToolkit* iToolkit);
	virtual void RegisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef);
	virtual void UnregisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager);

public:
    // Menu And Toolbar
    virtual void FillExtender(FBaseToolkit* iToolkit, TSharedPtr<FExtender>& ioExtender);

public:
    // Listeners
    virtual void OnToolkitInitialized(FBaseToolkit* iToolkit);

protected:
	// Layout
	virtual TSharedPtr<FTabManager::FLayout> CreateLayout();

public:
    // Getters
	TSharedRef<FTabManager::FLayout> GetLayout();
    virtual FName GetLayoutName() = 0;

private:
    FOdysseyEditor*                          mEditor;
	TSharedPtr<FTabManager::FLayout>         mLayout;

protected:
    //listing all tabs and managing the fact that a tab ptr can change through the Init process
    TArray<TAttribute<TSharedPtr<FOdysseyEditorTab>>> mTabs;
};

