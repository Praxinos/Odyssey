// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/LayerStack/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/LayerStack/Cells/CellImageVector/SOdysseyAnimationCellImageVector.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "OdysseyAnimation.h"
#include "UObject/OdysseyObjectEditorUtils.h"

SOdysseyAnimationLayerImageVectorTimeline::~SOdysseyAnimationLayerImageVectorTimeline()
{
}

SOdysseyAnimationLayerImageVectorTimeline::SOdysseyAnimationLayerImageVectorTimeline()
{
}

void
SOdysseyAnimationLayerImageVectorTimeline::Construct(
    const FArguments& InArgs,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
)
{
    ensure(iAnimationLayerImageVector);
    SOdysseyAnimationLayerImageTimeline::FArguments args;
    args.DisplayOptions(InArgs._DisplayOptions);
    SOdysseyAnimationLayerImageTimeline::Construct(args, iExtension, iAnimationLayerImageVector);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorTimeline::OnGenerateCellWidget(UOdysseyAnimationCell* iCell)
{
    if (!iCell)
    {
        return SNew(SOdysseyAnimationCellImageVector, Cast<UOdysseyAnimationCellImageVector>(iCell))
            .Clipping(EWidgetClipping::ClipToBoundsAlways)
            .ShowContent(this, &SOdysseyAnimationLayerImageVectorTimeline::GetShowCellContent); //DefaultCell, this can be called when creating cells, because the celle does not really exist yet
    }
    if (iCell->IsA<UOdysseyAnimationCellImageVector>())
    {
        return SNew(SOdysseyAnimationCellImageVector, Cast<UOdysseyAnimationCellImageVector>(iCell))
            .Clipping(EWidgetClipping::ClipToBoundsAlways)
            .ShowContent(this, &SOdysseyAnimationLayerImageVectorTimeline::GetShowCellContent);
    }
    else if (iCell->IsA<UOdysseyAnimationCellImageStagger>())
    {
        return SNew(SOdysseyAnimationCellImageStagger, Cast<UOdysseyAnimationCellImageStagger>(iCell), mExtension)
            .ShowContent(this, &SOdysseyAnimationLayerImageVectorTimeline::GetShowCellContent);
    }

    return SNullWidget::NullWidget;
}

FReply
SOdysseyAnimationLayerImageVectorTimeline::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    UOdysseyLayerStack* layerStack = mLayer->GetLayerStack();
    if (!layerStack)
        return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    if (layerStack->CurrentLayer.Get() == mLayer)
        return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, GET_MEMBER_NAME_CHECKED(UOdysseyLayerStack, CurrentLayer), mLayer);

    return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}

bool
SOdysseyAnimationLayerImageVectorTimeline::GetShowCellContent() const
{
    if (mLayer->IsLockedRecursively())
        return false;

    return DisplayOptions();
}
