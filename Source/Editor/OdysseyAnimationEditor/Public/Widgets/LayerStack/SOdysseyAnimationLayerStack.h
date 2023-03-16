// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"

/**
 * Implements the Animation Layer stack widget
 */

class UOdysseyAnimation;
class UOdysseyAnimationPlayer;
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerStack
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerStack)
        {}
        SLATE_ARGUMENT( UOdysseyAnimation*, Animation )
        SLATE_ARGUMENT( UOdysseyAnimationPlayer*, Player )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationLayerStack();
    SOdysseyAnimationLayerStack();
    
    void Construct(const FArguments& InArgs);

private:
    TSharedRef<ITableRow> OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable);

private:
    float GetZoom() const;
    float GetOffset() const;

private:
    UOdysseyAnimation* mAnimation;
    UOdysseyAnimationPlayer* mPlayer;
    TSharedPtr<SOdysseyLayerStackTreeView> mTreeView;
    float mZoom;
    float mOffset;
};
