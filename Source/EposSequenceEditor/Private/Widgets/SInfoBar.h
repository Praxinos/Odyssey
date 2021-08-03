// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs);

private:
};
