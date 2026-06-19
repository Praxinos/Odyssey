// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "ArianeEditorTab.h"

class FArianeEditor;
struct FArianeGroup;
class SArianeEditorSceneTreeView;
class SArianeEditorSceneDetailsView;

class ARIANEEDITOR_API FArianeEditorSceneTreeViewTab :
    public FArianeEditorTab
{
    public:
        static const FName& StaticId();

    public:
        // Construction / Destruction
        virtual ~FArianeEditorSceneTreeViewTab();
        FArianeEditorSceneTreeViewTab( FArianeEditor* Editor );

    protected:
        // FOdysseyEditorTab interface
        virtual const FName& GetId() const override;
        virtual TSharedPtr<SWidget> CreateWidget() override;

    public:
        // Event Listeners
        int WidgetIndex() const;

    private:
        FArianeGroup* GetRootGroup() const;
        void OnTransactCurrentFrame(TOptional<int> Frame) const;

    private:
        FArianeEditor* Editor;
};
