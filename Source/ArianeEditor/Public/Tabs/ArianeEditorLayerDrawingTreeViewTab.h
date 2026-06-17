// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#ifdef unused

#pragma once

#include "OdysseyEditorTab.h"
#include <ULIS>

class FArianeEditor;
class FOdysseyVectorGroupPaint;
class SArianeEditorVectorSceneTreeView;
class SArianeEditorVectorSceneDetailsView;

class ODYSSEYPAINTEREDITOR_API FArianeEditorVectorSceneTreeViewTab :
    public FOdysseyEditorTab
{
    public:
        static const FName& StaticId();

    public:
        // Construction / Destruction
        virtual ~FArianeEditorVectorSceneTreeViewTab();
        FArianeEditorVectorSceneTreeViewTab( FArianeEditor* iEditor );

    protected:
        // FOdysseyEditorTab interface
        virtual const FName& GetId() const override;
        virtual TSharedPtr<SWidget> CreateWidget() override;

    public:
        // Event Listeners
        int WidgetIndex() const;

    private:
        FOdysseyVectorGroupPaint* GetScene() const;
        void OnTransactCurrentFrame(TOptional<int> iFrame) const;
        uint64 GetVectorHUDFlags() const;

    private:
        FArianeEditor* mEditor;
};

#endif
