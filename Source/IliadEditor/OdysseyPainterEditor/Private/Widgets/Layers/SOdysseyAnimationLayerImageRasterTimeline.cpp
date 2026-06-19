// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationLayerImageRasterTimeline.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "SOdysseyAnimationCellImageStagger.h"
#include "SOdysseyAnimationCellImageRaster.h"
#include "OdysseyLayerCellImageStagger.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageRasterShortcuts.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimation.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageRasterTimeline::~SOdysseyAnimationLayerImageRasterTimeline()
{
}

SOdysseyAnimationLayerImageRasterTimeline::SOdysseyAnimationLayerImageRasterTimeline()
{
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterTimeline::OnGenerateCellWidget(UOdysseyLayerCell* iCell)
{
    if (!iCell)
    {
        return SNew(SOdysseyAnimationCellImageRaster, Cast<UOdysseyAnimationCellImageRaster>(iCell))
            .Clipping(EWidgetClipping::ClipToBoundsAlways);
    }
    if (iCell->IsA<UOdysseyAnimationCellImageRaster>())
    {
        return SNew(SOdysseyAnimationCellImageRaster, Cast<UOdysseyAnimationCellImageRaster>(iCell))
            .Clipping(EWidgetClipping::ClipToBoundsAlways);
    }
    else if (iCell->IsA<UOdysseyLayerCellImageStagger>())
    {
        return SNew(SOdysseyAnimationCellImageStagger, Cast<UOdysseyLayerCellImageStagger>(iCell))
            .TimelinePosition(mTimelinePosition);
    }

    return SNullWidget::NullWidget;
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    UOdysseyLayerStack* layerStack = mLayer->GetLayerStack();
    if (!layerStack)
        return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    if (layerStack->GetCurrentLayer() == mLayer)
        return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    layerStack->SetCurrentLayer(mLayer);

    return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}

void
SOdysseyAnimationLayerImageRasterTimeline::BuildContextMenu(TSharedRef<FUICommandList> CommandList, FMenuBuilder& MenuBuilder, FFrameNumber iClickedFrame)
{
    SOdysseyAnimationLayerImageTimeline::BuildContextMenu(CommandList, MenuBuilder, iClickedFrame);

    mAnimationTimelineCellImageRasterShortcuts = MakeShared<FOdysseyAnimationTimelineCellImageRasterShortcuts>(mLayer->GetAnimation());
    mAnimationTimelineCellImageRasterShortcuts->MapActionsToCommandList(CommandList);

    MenuBuilder.AddMenuEntry(
        FOdysseyPainterEditorAnimationCommands::Get().CrossFade,
        NAME_None,
        LOCTEXT("timeline-cells.context-menu.cross-fade.name", "Cross Fade")
    );
}

#undef LOCTEXT_NAMESPACE
