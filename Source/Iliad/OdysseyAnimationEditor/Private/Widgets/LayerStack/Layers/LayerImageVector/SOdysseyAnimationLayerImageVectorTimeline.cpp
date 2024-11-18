// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/LayerStack/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/LayerStack/Cells/CellImageVector/SOdysseyAnimationCellImageVector.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyAnimation.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "HUD/OdysseyVectorHUD.h"

SOdysseyAnimationLayerImageVectorTimeline::~SOdysseyAnimationLayerImageVectorTimeline()
{
}

SOdysseyAnimationLayerImageVectorTimeline::SOdysseyAnimationLayerImageVectorTimeline()
{
}

EVisibility
SOdysseyAnimationLayerImageVectorTimeline::GetRowVisibility(FName iRow) const
{
    if (iRow == "Inbetweening")
    {
        FOdysseyPainterEditor* editor = mEditor.Get();
        if (!editor)
            return EVisibility::Collapsed;

        return ( editor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) ? EVisibility::Visible : EVisibility::Collapsed;
    }

    return SOdysseyAnimationLayerImageTimeline::GetRowVisibility(iRow);
}

EVisibility
SOdysseyAnimationLayerImageVectorTimeline::IsInbetweeningTimelineVisible() const
{
    FOdysseyPainterEditor* editor = mEditor.Get();
    if (!editor)
        return EVisibility::Collapsed;

    return ( editor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) ? EVisibility::Visible : EVisibility::Collapsed;
}

TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>
SOdysseyAnimationLayerImageVectorTimeline::GetInbetweeningListView()
{
    return mInbetweeningListView;
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorTimeline::OnGenerateCellWidget(UOdysseyAnimationCell* iCell)
{
    if (!iCell)
    {
        return SNew(SOdysseyAnimationCellImageVector, Cast<UOdysseyAnimationCellImageVector>(iCell))
            .Clipping(EWidgetClipping::ClipToBoundsAlways);
    }
    if (iCell->IsA<UOdysseyAnimationCellImageVector>())
    {
        return SNew(SOdysseyAnimationCellImageVector, Cast<UOdysseyAnimationCellImageVector>(iCell))
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

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorTimeline::GenerateWidget( const FName& iRow, const FName& iColumn )
{
    ensure(iColumn == "Timeline");

    if (iRow == "Inbetweening")
    {
        return GenerateInbetweeningRowTimelineWidget();
    }

    return SOdysseyAnimationLayerImageTimeline::GenerateWidget( iRow, iColumn );
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorTimeline::GenerateInbetweeningRowTimelineWidget()
{
    return SAssignNew( mInbetweeningListView, SOdysseyAnimationLayerImageVectorTimelineInbetweening
                                , Cast<UOdysseyAnimationLayerImageVector>(mLayer) )
                            .TimelinePosition(mTimelinePosition)
                            .Visibility( this, &SOdysseyAnimationLayerImageVectorTimeline::IsInbetweeningTimelineVisible );
}
