// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"
#include "OdysseyAnimationEditorTimeline.h"

/**
 * Implements the Animation Layer stack widget
 */

class FOdysseyAnimationEditorExtension;
class SOdysseyLayerStackTreeView;
class UOdysseyAnimationLayerStack;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerStack
    : public SCompoundWidget
{
    SLATE_DECLARE_WIDGET(SOdysseyAnimationLayerStack, SCompoundWidget)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerStack)
        {}
        SLATE_DEFAULT_SLOT(FArguments, Content)
        SLATE_ATTRIBUTE( UOdysseyAnimationLayerStack*, LayerStack )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationLayerStack();
    SOdysseyAnimationLayerStack();
    
    void Construct(const FArguments& InArgs, FOdysseyAnimationEditorExtension* iAnimationExtension);
    TSharedPtr<SOdysseyLayerStackTreeView> GetTreeView() const;

private:
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

private:
    TSharedRef<ITableRow> OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable);
    void OnTimelineScrollBarScrolled(float iOffset);
    void RebuildWidgets();

    void OnLayerAdded(UOdysseyLayer* iLayer);
    float PlaybackFramesPerSecond() const;

    EOdysseyTimelineTool GetCurrentTool() const;
    void OnToolChecked(EOdysseyTimelineTool iTool, ECheckBoxState iState);

private:
    FOdysseyAnimationEditorExtension* mExtension;
    TSlateAttribute<UOdysseyAnimationLayerStack*> mLayerStack;
    TSharedPtr<SOdysseyLayerStackTreeView> mTreeView;
    TSharedPtr<SScrollBar> mTimelineScrollBar;
    TSharedPtr<class SOdysseyAnimationTimelineControl> mTimelineControl;
};
