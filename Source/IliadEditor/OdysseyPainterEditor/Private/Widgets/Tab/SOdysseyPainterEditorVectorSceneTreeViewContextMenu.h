// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"

class SOdysseyPainterEditorVectorSceneTreeView;
class FOdysseyVectorGroupPaint;
class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorVectorSceneTreeViewContextMenu
{
    public:
        // FOdysseyEditorTab interface
        static TSharedPtr<SWidget> CreateWidget( SOdysseyPainterEditorVectorSceneTreeView* iTreeView );

    protected:
        static bool CanAddInbetweener( FOdysseyVectorGroupPaint* iScene );
        static bool CanAlterInbetweener( FOdysseyVectorGroupPaint* iScene );
        static bool CanUngroup( FOdysseyVectorGroupPaint* iScene );
        static void Group( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iPaintGroup );
};
