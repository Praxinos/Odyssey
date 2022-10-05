// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "ToolMenuOwner.h"

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
    virtual void ExtendMenu( FToolMenuOwner iOwnerFName, FName iMenuName );

public:
    // Listeners
    virtual void OnToolkitInitialized(FBaseToolkit* iToolkit);

protected:
	// Layout

    //There is 2 ways to create the layout
    // - As a FTabManager::FLayout which has proper tabs layed out in a window, used by standalone editors like the Texture Editor
    // - As a Widget, which is a very customized layout used for specific purposes like ViewportDrawingEditor layout
    // each of these methods can return nullptr
	virtual TSharedPtr<FTabManager::FLayout> CreateLayout();
    virtual TSharedPtr<SWidget> CreateWidget();

public:
    // Getters
	TSharedRef<FTabManager::FLayout> GetLayout();
    TSharedRef<SWidget> GetWidget();
    virtual FName GetLayoutName() = 0;

private:
    FOdysseyEditor*                         mEditor;
	TSharedPtr<FTabManager::FLayout>        mLayout;
    TSharedPtr<SWidget>                     mWidget;

protected:
    //listing all tabs and managing the fact that a tab ptr can change through the Init process
    TArray<TAttribute<TSharedPtr<FOdysseyEditorTab>>> mTabs;
};

