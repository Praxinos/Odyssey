// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

/**
 * Implements the Scene Tree View Widget
 */
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorToolbarCommon
    : public SCompoundWidget
{
    public:
        SLATE_BEGIN_ARGS(SOdysseyPainterEditorToolbarCommon)
            {}
        SLATE_END_ARGS()

    public:
        // Construction / Destruction
        ~SOdysseyPainterEditorToolbarCommon();
        SOdysseyPainterEditorToolbarCommon();

        void Construct(const FArguments& InArgs);
};
