// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"

class SBox;

/** A custom widget that comprises a combo box displaying all available storyboard viewport options
*
*   This class is not really used, only the static function is really used...
*/
class SStoryboardViewportSettings
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SStoryboardViewportSettings )
    {}
    SLATE_END_ARGS()

    /** Construct this widget */
    void Construct(const FArguments& InArgs);

public:
    /** Generate menu content for the combo button */
    static TSharedRef<SWidget> GetMenuContent();

private:
    static FReply OnCameraBoundsColorBlockMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, TSharedPtr<SBox> iWidget);
};
