// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"
#include <ULIS>

class FOdysseyPainterEditor;
class SOdysseyPainterEditorVectorSceneTreeView;
class FVectorSceneTreeViewItem;
class FOdysseyVectorScene;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorSceneTreeViewTab :
    public FOdysseyEditorTab
{
    public:
        // Construction / Destruction
        virtual ~FOdysseyPainterEditorVectorSceneTreeViewTab();
        FOdysseyPainterEditorVectorSceneTreeViewTab( FOdysseyPainterEditor* iEditor );

    protected:
        // FOdysseyEditorTab interface
        virtual TSharedPtr<SWidget> CreateWidget() override;

    public:
        // Event Listeners
        void OnRefresh( FOdysseyVectorScene* iScene );
        void Update( FOdysseyVectorScene* iScene );

    private:
        FOdysseyPainterEditor* mEditor;
        TSharedPtr<SOdysseyPainterEditorVectorSceneTreeView> mVectorSceneTreeView;
};
