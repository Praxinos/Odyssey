// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineTreeView.h"

#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineShortcuts.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "LayerFolder/OdysseyAnimationLayerFolder.h"
#include "OdysseyAnimationLayerStack.h"
#include "SOdysseyAnimationTimelineHeader.h"
#include "Widgets/Animation/Timeline/Layers/LayerFolder/SOdysseyAnimationLayerFolderTimeline.h"
#include "Widgets/Animation/Timeline/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "Widgets/Animation/Timeline/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "LayerFolder/OdysseyAnimationLayerFolder.h"
#include "Framework/Commands/UICommandList.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationTimelineTreeView::Construct(const FArguments& InArgs)
{
    mCurrentFrame = InArgs._CurrentFrame;
    mTimelinePosition = InArgs._TimelinePosition;
    mLayerStack = InArgs._LayerStack;
    mOnActivateOutOfPegs = InArgs._OnActivateOutOfPegs;
    mOnInactivateOutOfPegs = InArgs._OnInactivateOutOfPegs;
    mOnIsOutOfPegsChecked = InArgs._OnIsOutOfPegsChecked;

    if (!mLayerStack)
        return;

    mTimelineShortcuts = MakeShared<FOdysseyAnimationTimelineShortcuts>(mLayerStack->GetAnimation(), InArgs._CurrentFrame, InArgs._OnTransactCurrentFrame);

    TArray<SHeaderRow::FColumn::FArguments> columns = {
        SHeaderRow::Column("Timeline")
        .DefaultLabel(FText())
        .VAlignCell(VAlign_Fill)
        .HAlignCell(HAlign_Fill)
        .HeaderContentPadding(FMargin(0.f))
        [
            SNew(SBox)
            .HeightOverride(25.f)
            [
                SNew(SOdysseyAnimationTimelineHeader)
                .Animation(mLayerStack->GetAnimation())
                .TimelinePosition(mTimelinePosition)
                .OnScrubStart(InArgs._OnScrubStart)
                .OnScrubEnd(InArgs._OnScrubEnd)
                .OnCurrentFrameChanged(InArgs._OnCurrentFrameChanged)
                .OnCurrentFrameCommited(InArgs._OnCurrentFrameCommited)
            ]
        ]
    };

    SOdysseyLayerStackTreeView::Construct(
        SOdysseyLayerStackTreeView::FArguments()
        .LayerStack(mLayerStack)
        .OnGenerateRow( this, &SOdysseyAnimationTimelineTreeView::OnGenerateRow )
        .Columns(columns)
        .ExternalScrollbar(InArgs._ExternalScrollbar)
        .OnTreeViewScrolled(InArgs._OnTreeViewScrolled)
    );
}

FReply
SOdysseyAnimationTimelineTreeView::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
    if (mTimelineShortcuts->GetCommandList()->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SCompoundWidget::OnKeyDown(iGeometry, iKeyEvent);
}

TSharedRef<ITableRow>
SOdysseyAnimationTimelineTreeView::OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iLayer);

    UClass* layerClass = iLayer->GetClass();
    if (layerClass == UOdysseyAnimationLayerFolder::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerFolderTimeline, SharedThis(this), Cast<UOdysseyAnimationLayerFolder>(iLayer));
    }
    if (layerClass == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageRasterTimeline, SharedThis(this), Cast<UOdysseyAnimationLayerImageRaster>(iLayer))
            .CurrentFrame(mCurrentFrame)
            .TimelinePosition(mTimelinePosition)
            .OnActivateOutOfPegs(mOnActivateOutOfPegs)
            .OnInactivateOutOfPegs(mOnInactivateOutOfPegs)
            .OnIsOutOfPegsChecked(mOnIsOutOfPegsChecked);
    }
    if (layerClass == UOdysseyAnimationLayerImageVector::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageVectorTimeline, SharedThis(this), Cast<UOdysseyAnimationLayerImageVector>(iLayer))
            .CurrentFrame(mCurrentFrame)
            .TimelinePosition(mTimelinePosition)
            .OnActivateOutOfPegs(mOnActivateOutOfPegs)
            .OnInactivateOutOfPegs(mOnInactivateOutOfPegs)
            .OnIsOutOfPegsChecked(mOnIsOutOfPegsChecked);
    }

    return SNew(STableRow<UOdysseyLayer*>, iOwnerTable);;
}



FCursorReply
SOdysseyAnimationTimelineTreeView::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const
{
    FCursorReply reply = SOdysseyLayerStackTreeView::OnCursorQuery( MyGeometry, CursorEvent );
    if (reply.IsEventHandled())
        return reply;

    return FCursorReply::Cursor( EMouseCursor::Default );
}

#undef LOCTEXT_NAMESPACE
