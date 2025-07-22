// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/Animation/Timeline/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/Animation/Timeline/Cells/CellImageVector/SOdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "Widgets/Animation/Timeline/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyLayerCellImageStagger.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorModule.h"
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
        FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
        FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mLayer->GetAnimation());
        if (!editor)
            return EVisibility::Collapsed;

        return ( editor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) ? EVisibility::Visible : EVisibility::Collapsed;
    }

    return SOdysseyAnimationLayerImageTimeline::GetRowVisibility(iRow);
}

EVisibility
SOdysseyAnimationLayerImageVectorTimeline::IsInbetweeningTimelineVisible() const
{
    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mLayer->GetAnimation());
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
SOdysseyAnimationLayerImageVectorTimeline::OnGenerateCellWidget(UOdysseyLayerCell* iCell)
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
    else if (iCell->IsA<UOdysseyLayerCellImageStagger>())
    {
        return SNew(SOdysseyAnimationCellImageStagger, Cast<UOdysseyLayerCellImageStagger>(iCell))
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

    if (layerStack->GetCurrentLayer() == mLayer)
        return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    layerStack->SetCurrentLayer(mLayer);

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
