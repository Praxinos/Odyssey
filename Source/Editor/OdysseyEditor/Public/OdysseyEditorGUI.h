// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "ToolMenuOwner.h"

class FOdysseyEditor;
class FOdysseyEditorTab;
class FOdysseyEditorContextMenu;

/*
#define ODYSSEY_ADD_TAB(var, class, ...) { var = MakeShareable(new class(__VA_ARGS__)); mTabs.Add(TAttribute<TSharedPtr<FOdysseyEditorTab>>::Create([&](){ return var; })); }
#define ODYSSEY_SET_TAB(var, class, ...) { var = MakeShareable(new class(__VA_ARGS__)); }
*/

/**
 * Implements an Editor GUI.
 */

/*
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
    
public:
    // Menu And Toolbar
    
protected:
	// Layout
}; 

*/