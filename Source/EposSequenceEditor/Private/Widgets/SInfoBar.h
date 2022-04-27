// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
