// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyEditorTab.h"
#include <ULIS>

class FOdysseyPainterEditor;
class FOdysseyVectorGroupPaint;
class SOdysseyPainterEditorVectorSceneTreeView;
class SOdysseyPainterEditorVectorSceneDetailsView;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorSceneTreeViewTab :
    public FOdysseyEditorTab
{
    public:
        static const FName& StaticId();

    public:
        // Construction / Destruction
        virtual ~FOdysseyPainterEditorVectorSceneTreeViewTab();
        FOdysseyPainterEditorVectorSceneTreeViewTab( FOdysseyPainterEditor* iEditor );

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
        FOdysseyPainterEditor* mEditor;
};
