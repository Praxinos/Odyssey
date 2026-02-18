// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class FArianeEditor;
class UArianeEditorTool;

/**
 * Implements the Scene Tree View Widget
 */
class ARIANEEDITOR_API SArianeEditorToolPanel
    : public SCompoundWidget
{
    public:
        SLATE_BEGIN_ARGS(SArianeEditorToolPanel)
            {}
            SLATE_ARGUMENT(UArianeEditorTool*, CurrentTool)
        SLATE_END_ARGS()

    public:
        // Construction / Destruction
        ~SArianeEditorToolPanel();
        SArianeEditorToolPanel();

        void Construct(const FArguments& InArgs, FArianeEditor* iEditor);

    protected:
        FArianeEditor* mEditor;
};
