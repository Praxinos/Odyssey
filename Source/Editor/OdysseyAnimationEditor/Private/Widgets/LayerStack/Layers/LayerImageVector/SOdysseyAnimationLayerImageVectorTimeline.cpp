// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/LayerStack/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/LayerStack/Cells/CellImageVector/SOdysseyAnimationCellImageVector.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageVectorTimeline"

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
    SOdysseyAnimationLayerImageTimeline::Construct(iExtension, iAnimationLayerImageVector);
}

TSharedRef<FOdysseyAnimationCell>
SOdysseyAnimationLayerImageVectorTimeline::OnCreateCell()
{
    UOdysseyAnimation* animation = mLayer->GetAnimation();
    return FOdysseyAnimationCellImageVector::Create(Cast<UOdysseyAnimationLayerImageVector>(mLayer), 1, animation->Width(), animation->Height());
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorTimeline::OnGenerateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell)
{
    if (iCell->GetType() == FOdysseyAnimationCellImageVector::StaticType())
        return SNew(SOdysseyAnimationCellImageVector);
    else if (iCell->GetType() == FOdysseyAnimationCellImageStagger::StaticType())
        return SNew(SOdysseyAnimationCellImageStagger, StaticCastSharedPtr<FOdysseyAnimationCellImageStagger>(iCell), mExtension);

    return SNullWidget::NullWidget;
}

#undef LOCTEXT_NAMESPACE
