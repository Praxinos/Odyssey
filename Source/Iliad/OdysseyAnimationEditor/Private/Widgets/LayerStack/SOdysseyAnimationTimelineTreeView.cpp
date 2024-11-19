// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#include "Widgets/LayerStack/SOdysseyAnimationTimelineTreeView.h"

#include "Shortcuts/Timeline/OdysseyAnimationTimelineShortcuts.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "SOdysseyAnimationTimelineHeader.h"
#include "Widgets/LayerStack/Layers/LayerFolder/SOdysseyAnimationLayerFolderTimeline.h"
#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolder.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationTimelineTreeView::Construct(const FArguments& InArgs)
{
    mTimelinePosition = InArgs._TimelinePosition;
    mLayerStack = InArgs._LayerStack;
    mPlayer = InArgs._Player;
    mOnActivateOutOfPegs = InArgs._OnActivateOutOfPegs;
    mOnInactivateOutOfPegs = InArgs._OnInactivateOutOfPegs;
    mOnIsOutOfPegsChecked = InArgs._OnIsOutOfPegsChecked;

    mTimelineShortcuts = MakeShared<FOdysseyAnimationTimelineShortcuts>(mLayerStack);

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
                .Player(mPlayer)
                .TimelinePosition(mTimelinePosition)
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
            .TimelinePosition(mTimelinePosition)
            .OnActivateOutOfPegs(mOnActivateOutOfPegs)
            .OnInactivateOutOfPegs(mOnInactivateOutOfPegs)
            .OnIsOutOfPegsChecked(mOnIsOutOfPegsChecked);
    }
    if (layerClass == UOdysseyAnimationLayerImageVector::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageVectorTimeline, SharedThis(this), Cast<UOdysseyAnimationLayerImageVector>(iLayer))
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
