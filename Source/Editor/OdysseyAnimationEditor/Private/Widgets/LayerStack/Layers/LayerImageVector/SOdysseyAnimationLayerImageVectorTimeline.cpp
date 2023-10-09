// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"

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
    return SNew(SOdysseyAnimationLayerImageVectorCell);
}

#undef LOCTEXT_NAMESPACE
