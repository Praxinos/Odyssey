// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "Widgets/LayerStack/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/LayerStack/Cells/CellImageRaster/SOdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "Shortcuts/Timeline/OdysseyAnimationTimelineCellImageRasterShortcuts.h"

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
    args.IsCollapsed(InArgs._IsCollapsed);
    SOdysseyAnimationLayerImageTimeline::Construct(args, iExtension, iAnimationLayerImageRaster);
}

TSharedRef<FOdysseyAnimationCell>
SOdysseyAnimationLayerImageRasterTimeline::OnCreateCell()
{
    UOdysseyAnimation* animation = mLayer->GetAnimation();
    return FOdysseyAnimationCellImageRaster::Create(Cast<UOdysseyAnimationLayerImageRaster>(mLayer), 1, animation->Width(), animation->Height(), animation->Format());
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterTimeline::OnGenerateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell)
{
    if (!iCell)
        return SNew(SOdysseyAnimationCellImageRaster); //DefaultCell, this can be called when creating cells, because the celle does not really exist yet
    if (iCell->GetType() == FOdysseyAnimationCellImageRaster::StaticType())
        return SNew(SOdysseyAnimationCellImageRaster);
    else if (iCell->GetType() == FOdysseyAnimationCellImageStagger::StaticType())
        return SNew(SOdysseyAnimationCellImageStagger, StaticCastSharedPtr<FOdysseyAnimationCellImageStagger>(iCell), mExtension)
            .ShowContent(this, &SOdysseyAnimationLayerImageRasterTimeline::GetShowStaggerCellContent);

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

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, "CurrentLayer", TSoftObjectPtr<UOdysseyLayer>(mLayer));

    return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}

bool
SOdysseyAnimationLayerImageRasterTimeline::GetShowStaggerCellContent() const
{
    if (mLayer->GetIsLocked())
        return false;

    return !IsCollapsed();
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
        TEXT("ConvertToStagger"),
        EExtensionHook::After,
        commandList,
        FMenuExtensionDelegate::CreateLambda(
            [](FMenuBuilder& iMenuBuilder)
            {
                iMenuBuilder.AddMenuEntry(FOdysseyAnimationEditorCommands::Get().ConvertToRasterCell);
            }
        )
    );

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