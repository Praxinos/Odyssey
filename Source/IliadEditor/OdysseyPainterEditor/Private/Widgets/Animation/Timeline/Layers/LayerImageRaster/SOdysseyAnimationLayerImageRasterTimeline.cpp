// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "Widgets/Animation/Timeline/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/Animation/Timeline/Cells/CellImageRaster/SOdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageRasterShortcuts.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
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
SOdysseyAnimationLayerImageRasterTimeline::OnGenerateCellWidget(UOdysseyAnimationCell* iCell)
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
    else if (iCell->IsA<UOdysseyAnimationCellImageStagger>())
    {
        return SNew(SOdysseyAnimationCellImageStagger, Cast<UOdysseyAnimationCellImageStagger>(iCell))
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

    if (layerStack->CurrentLayer.Get() == mLayer)
        return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, GET_MEMBER_NAME_CHECKED(UOdysseyLayerStack, CurrentLayer), mLayer);

    return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}

TSharedPtr<FExtender>
SOdysseyAnimationLayerImageRasterTimeline::ExtendContextMenu()
{
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
    mAnimationTimelineCellImageRasterShortcuts = MakeShared<FOdysseyAnimationTimelineCellImageRasterShortcuts>(mLayer->GetLayerStack());
    mAnimationTimelineCellImageRasterShortcuts->MapActionsToCommandList(commandList);

    TSharedRef<FExtender> extender = MakeShared<FExtender>();
    extender->AddMenuExtension
    (
        TEXT("Cells"),
        EExtensionHook::After,
        commandList,
        FMenuExtensionDelegate::CreateLambda(
            [](FMenuBuilder& iMenuBuilder)
            {
                iMenuBuilder.AddMenuEntry(
                    FOdysseyPainterEditorAnimationCommands::Get().CrossFade,
                    NAME_None,
                    LOCTEXT("timeline-cells.context-menu.cross-fade.name", "Cross Fade")
                );
            }
        )
    );

    return extender;
}

#undef LOCTEXT_NAMESPACE
