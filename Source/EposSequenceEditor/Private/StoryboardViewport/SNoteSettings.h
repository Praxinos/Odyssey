// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"


/** A custom widget that comprises a combo box displaying all available note options */
class SNoteSettings
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SNoteSettings )
    {}
    SLATE_END_ARGS()

    /** Construct this widget */
    void Construct(const FArguments& InArgs);

private:

    /** Generate menu content for the combo button */
    TSharedRef<SWidget> GetMenuContent();
};
