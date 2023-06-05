// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"

/**
 * Implements the Animation Layer stack widget
 */

class FOdysseyAnimationEditor;
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerStack
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerStack)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationLayerStack();
    SOdysseyAnimationLayerStack();
    
    void Construct(const FArguments& InArgs, FOdysseyAnimationEditor* iAnimationEditor);
    TSharedPtr<SOdysseyLayerStackTreeView> GetTreeView() const;

private:
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

private:
    TSharedRef<ITableRow> OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable);
    void OnTimelineScrollBarScrolled(float iOffset);

private:
    FOdysseyAnimationEditor* mEditor;
    TSharedPtr<SOdysseyLayerStackTreeView> mTreeView;
	TSharedPtr<SScrollBar> mTimelineScrollBar;
    TSharedPtr<class SOdysseyAnimationTimelineControl> mTimelineControl;
};
