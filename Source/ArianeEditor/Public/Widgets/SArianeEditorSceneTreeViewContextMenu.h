// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"

class SArianeEditorSceneTreeView;
struct FArianeGroup;
class FArianeEditor;

class ARIANEEDITOR_API SArianeEditorSceneTreeViewContextMenu
{
    public:
        // FOdysseyEditorTab interface
        static TSharedPtr<SWidget> CreateWidget( SArianeEditorSceneTreeView* TreeView );

    protected:
        static bool CanUngroup( FArianeGroup* Scene );
        static void Group( FArianeEditor* iEditor, FArianeGroup* Scene );
};
