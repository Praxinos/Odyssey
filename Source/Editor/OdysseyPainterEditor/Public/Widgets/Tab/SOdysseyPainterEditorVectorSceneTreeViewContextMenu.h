// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"

class SOdysseyPainterEditorVectorSceneTreeView;

class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorVectorSceneTreeViewContextMenu
{
    public:
        // FOdysseyEditorTab interface
        static TSharedPtr<SWidget> CreateWidget( SOdysseyPainterEditorVectorSceneTreeView* iTreeView );
};
