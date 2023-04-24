// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SOdysseyLayerRow.h"

class SOdysseyAnimationLayerStack;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerRow
    : public SOdysseyLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerRow)
        {}
    SLATE_END_ARGS()

public:
    void Construct(
        const FArguments& iArgs,
        TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget,
        class UOdysseyAnimationLayer* iLayer
    );

public:
    TSharedPtr<SOdysseyAnimationLayerStack> GetLayerStackWidget() const;

public:
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;

protected:
    virtual TSharedRef<SWidget> GenerateTimelineWidget();

private:
    TWeakPtr<SOdysseyAnimationLayerStack> mLayerStackWidget;
};
