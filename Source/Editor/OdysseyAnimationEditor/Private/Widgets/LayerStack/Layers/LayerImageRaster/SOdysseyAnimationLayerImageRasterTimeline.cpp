// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "Widgets/LayerStack/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/LayerStack/Cells/CellImageRaster/SOdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "Shortcuts/Timeline/OdysseyAnimationTimelineCellImageRasterShortcuts.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimation.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageRasterTimeline::~SOdysseyAnimationLayerImageRasterTimeline()
{
}

SOdysseyAnimationLayerImageRasterTimeline::SOdysseyAnimationLayerImageRasterTimeline()
{
}

void
SOdysseyAnimationLayerImageRasterTimeline::Construct(
    const FArguments& InArgs,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
)
{
    ensure(iAnimationLayerImageRaster);
    SOdysseyAnimationLayerImageTimeline::FArguments args;
    args.DisplayOptions(InArgs._DisplayOptions);
    SOdysseyAnimationLayerImageTimeline::Construct(args, iExtension, iAnimationLayerImageRaster);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterTimeline::OnGenerateCellWidget(UOdysseyAnimationCell* iCell)
{
    if (!iCell)
	{
        return SNew(SOdysseyAnimationCellImageRaster, Cast<UOdysseyAnimationCellImageRaster>(iCell))
			.Clipping(EWidgetClipping::ClipToBoundsAlways)
			.ShowContent(this, &SOdysseyAnimationLayerImageRasterTimeline::GetShowCellContent); //DefaultCell, this can be called when creating cells, because the celle does not really exist yet
	}
    if (iCell->IsA<UOdysseyAnimationCellImageRaster>())
	{
        return SNew(SOdysseyAnimationCellImageRaster, Cast<UOdysseyAnimationCellImageRaster>(iCell))
			.Clipping(EWidgetClipping::ClipToBoundsAlways)
			.ShowContent(this, &SOdysseyAnimationLayerImageRasterTimeline::GetShowCellContent);
	}
    else if (iCell->IsA<UOdysseyAnimationCellImageStagger>())
	{
        return SNew(SOdysseyAnimationCellImageStagger, Cast<UOdysseyAnimationCellImageStagger>(iCell), mExtension)
            .ShowContent(this, &SOdysseyAnimationLayerImageRasterTimeline::GetShowCellContent);
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

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, GET_MEMBER_NAME_CHECKED(UOdysseyLayerStack, CurrentLayer), TSoftObjectPtr<UOdysseyLayer>(mLayer));

    return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}

bool
SOdysseyAnimationLayerImageRasterTimeline::GetShowCellContent() const
{
    if (mLayer->IsLockedRecursively())
        return false;

    return DisplayOptions();
}

TSharedPtr<FExtender>
SOdysseyAnimationLayerImageRasterTimeline::ExtendContextMenu()
{
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
    mAnimationTimelineCellImageRasterShortcuts = MakeShared<FOdysseyAnimationTimelineCellImageRasterShortcuts>(mLayer->GetLayerStack(), mExtension);
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
                    FOdysseyAnimationEditorCommands::Get().CrossFade,
                    NAME_None,
                    LOCTEXT("timeline-cells.context-menu.cross-fade.name", "Cross Fade")
                );
            }
        )
    );

    return extender;
}

#undef LOCTEXT_NAMESPACE