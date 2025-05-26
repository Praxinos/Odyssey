// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Widgets/SCompoundWidget.h"

//---

class EPOSSEQUENCEEDITOR_API SInfoBar
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SInfoBar )
        {}
        SLATE_ATTRIBUTE( FText, Text )
        SLATE_ATTRIBUTE( FMargin, Padding )
        SLATE_EVENT( FPointerEventHandler, OnDoubleClicked )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs);

private:
};
