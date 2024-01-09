// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"

#include "Shortcuts/Timeline/OdysseyAnimationTimelineShortcuts.h"

SOdysseyAnimationLayerStackTreeView::SOdysseyAnimationLayerStackTreeView()
    : mTimelineShortcuts(nullptr)
    , mExtension(nullptr)
{
}

FOdysseyAnimationEditorExtension*
SOdysseyAnimationLayerStackTreeView::GetAnimationEditorExtension() const
{
    return mExtension;
}

void
SOdysseyAnimationLayerStackTreeView::Construct(const FArguments& InArgs, FOdysseyAnimationEditorExtension* iAnimationExtension)
{
    SOdysseyLayerStackTreeView::Construct(InArgs);
    mExtension = iAnimationExtension;
    mTimelineShortcuts = MakeShared<FOdysseyAnimationTimelineShortcuts>(SharedThis(this));
}

FReply
SOdysseyAnimationLayerStackTreeView::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
	if (mTimelineShortcuts->GetCommandList()->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SOdysseyLayerStackTreeView::OnKeyDown(iGeometry, iKeyEvent);
}

void
SOdysseyAnimationLayerStackTreeView::Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo)
{
    mExtension->Timeline()->SetSelectedFrames(FInt32Range::Empty());
    SOdysseyLayerStackTreeView::Private_SignalSelectionChanged(SelectInfo);
}

FReply
SOdysseyAnimationLayerStackTreeView::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
    mExtension->Timeline()->SetSelectedFrames(FInt32Range::Empty());
    return SOdysseyLayerStackTreeView::OnFocusReceived(MyGeometry, InFocusEvent);
}