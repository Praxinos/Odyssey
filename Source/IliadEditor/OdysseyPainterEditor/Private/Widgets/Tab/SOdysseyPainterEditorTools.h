// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

/**
 * Implements the Scene Tree View Widget
 */
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorTools
    : public SCompoundWidget
{
    public:
        SLATE_BEGIN_ARGS(SOdysseyPainterEditorTools)
            {}
            SLATE_ATTRIBUTE(FOdysseyPainterEditor*, Editor)
        SLATE_END_ARGS()

    public:
        // Construction / Destruction
        ~SOdysseyPainterEditorTools();
        SOdysseyPainterEditorTools();

        void Construct(const FArguments& InArgs);

    private:
        UOdysseyPainterEditorTool* GetCurrentTool() const;
        void OnToolSelected(UOdysseyPainterEditorTool* iTool);

    protected:
        TAttribute<FOdysseyPainterEditor*> mEditor;
};
