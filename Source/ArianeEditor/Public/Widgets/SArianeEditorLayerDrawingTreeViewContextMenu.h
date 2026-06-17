// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#ifdef unused

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"

class SArianeEditorVectorSceneTreeView;
class FOdysseyVectorGroupPaint;
class FArianeEditor;

class ODYSSEYPAINTEREDITOR_API SArianeEditorVectorSceneTreeViewContextMenu
{
    public:
        // FOdysseyEditorTab interface
        static TSharedPtr<SWidget> CreateWidget( SArianeEditorVectorSceneTreeView* iTreeView );

    protected:
        static bool CanAddInbetweener( FOdysseyVectorGroupPaint* iScene );
        static bool CanAlterInbetweener( FOdysseyVectorGroupPaint* iScene );
        static bool CanUngroup( FOdysseyVectorGroupPaint* iScene );
        static void Group( FArianeEditor* iEditor, FOdysseyVectorGroupPaint* iPaintGroup );
};

#endif
