// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

// From module OdysseyAnimationEditor
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
// From module OdysseyStyle
#include "OdysseyStyleSet.h"
// From module OdysseyVector
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorAnimationCell.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::~SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow()
{
}

SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow()
{
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::Construct( const typename STableRow<TSharedPtr<FInbetweeningListViewItem>>::FArguments& InArgs
                                                                   , const TSharedRef< STableViewBase >& InOwnerTableView
                                                                   , const TSharedPtr<FInbetweeningListViewItem> iITem )
{
    STableRow<TSharedPtr<FInbetweeningListViewItem>>::Construct( InArgs, InOwnerTableView );

    mInbetweenerTag = iITem.Get()->GetInbetweenerTag();

    //mCellsBox = SNew(SHorizontalBox);

    //SetContent( mCellsBox.ToSharedRef() );

    Update();
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::Update()
{
    // retrieve parent widget
    const TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> listView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
    // retrieve timing data
    int32 tagCellIndex = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetIndex();
    uint32 fromFrame = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetFrame();
    uint32 toFrame   = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetCellByIndex( fromFrame + mInbetweenerTag->GetInbetweenCount() + 1 )->GetFrame();
    // compute geometry
    FOdysseyAnimationEditorExtension* animationEditorExtension = listView.Get()->GetAnimationEditorExtension();
    float tagCellX = fromFrame * animationEditorExtension->Timeline()->GetFrameWidth();
    float tagCellW = toFrame * animationEditorExtension->Timeline()->GetFrameWidth();
}


int32
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::OnPaint( const FPaintArgs& Args
                                                                 , const FGeometry& AllottedGeometry
                                                                 , const FSlateRect& MyCullingRect
                                                                 , FSlateWindowElementList& OutDrawElements
                                                                 , int32 LayerId
                                                                 , const FWidgetStyle& InWidgetStyle
                                                                 , bool bParentEnabled ) const
{
    // retrieve parent widget
    const TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> listView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
    // retrieve timing data
    int32 tagCellIndex = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetIndex();
    int32 tagCellCount = mInbetweenerTag->GetInbetweenCount();
    uint32 fromFrame = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetFrame();
    uint32 toFrame   = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetCellByIndex( tagCellIndex + tagCellCount )->GetFrame();
    // compute geometry
    FOdysseyAnimationEditorExtension* animationEditorExtension = listView.Get()->GetAnimationEditorExtension();
    float tagCellX = fromFrame * animationEditorExtension->Timeline()->GetFrameWidth();
    float tagCellW = toFrame * animationEditorExtension->Timeline()->GetFrameWidth();
    //FVector2D widgetSize = MyCullingRect.GetSize();
    static FSlateBrush whiteBackgroundBrush;

	// Draw a current frame
	LayerId = SCompoundWidget::OnPaint( Args
                                      , AllottedGeometry
                                      , MyCullingRect
                                      , OutDrawElements
                                      , LayerId
                                      , InWidgetStyle
                                      , bParentEnabled );
	++LayerId;

    FLinearColor color = FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f );

	FSlateDrawElement::MakeBox( OutDrawElements
		                      , LayerId
		                      , AllottedGeometry.ToPaintGeometry( FVector2D( tagCellX, 0.0f )
		                                                        , FVector2D( tagCellW, 10/*AllottedGeometry.Size.Y*/ ) )
		                      , &whiteBackgroundBrush
		                      , ESlateDrawEffect::None
		                      , color
	);

    return LayerId;
}

#undef LOCTEXT_NAMESPACE
