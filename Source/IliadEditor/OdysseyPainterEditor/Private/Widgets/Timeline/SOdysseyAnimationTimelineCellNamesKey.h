// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UOdysseyLayerCell;
class FOdysseyPainterEditorAnimationTimelinePosition;

class SOdysseyAnimationTimelineCellNamesKey
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineCellNamesKey)
    {}
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
        SLATE_ATTRIBUTE( UOdysseyLayerCell*, Cell)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FText GetText() const;
    FSlateFontInfo GetFontInfo() const;
    FSlateColor GetColor() const;

    void OnNameCommited( const FText& iText, ETextCommit::Type iType );

private:
    TAttribute<UOdysseyLayerCell*> mCell;
};
