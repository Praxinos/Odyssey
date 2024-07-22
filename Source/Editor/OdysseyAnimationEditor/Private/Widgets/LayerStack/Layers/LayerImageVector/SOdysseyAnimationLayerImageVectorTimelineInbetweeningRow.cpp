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

    ChildSlot
    [
        SNew( STextBlock)
        .Text( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::GetInbetweenerTagInbetweenCount )
    ];

    Update();
}

FText
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::GetInbetweenerTagInbetweenCount() const
{
    return FText::FromString( FString::FromInt( (int32)mInbetweenerTag->GetInbetweenCount() ) );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::Update()
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
    const FGeometry& geometry = GetPaintSpaceGeometry();

    mBoxPos.X  = fromFrame * animationEditorExtension->Timeline()->GetFrameWidth();
    mBoxPos.Y  = 0.0f;
    mBoxSize.X = toFrame * animationEditorExtension->Timeline()->GetFrameWidth();
    //mBoxSize.Y = GetPaintSpaceGeometry().GetAbsoluteSize().Y;
    mBoxSize.Y = ChildSlot.GetWidget().Get().GetPaintSpaceGeometry().GetAbsoluteSize().Y;
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
    const FColor& inbetweenerTagColor = mInbetweenerTag->GetColor();
    static FSlateBrush defaultBrush;

	// Draw a current frame
	LayerId = SCompoundWidget::OnPaint( Args
                                      , AllottedGeometry
                                      , MyCullingRect
                                      , OutDrawElements
                                      , LayerId
                                      , InWidgetStyle
                                      , bParentEnabled );
	++LayerId;
    FLinearColor strokeColor = FLinearColor( 0.5f
                                           , 0.5f
                                           , 0.5f
                                           , 0.5f );
    FLinearColor fillColor = FLinearColor( inbetweenerTagColor.R
                                         , inbetweenerTagColor.G
                                         , inbetweenerTagColor.B
                                         , 0.25f );
    TArray< FVector2D > lines;

    lines.Reserve( 5 );
    lines.Push( FVector2D( mBoxPos.X             , mBoxPos.Y              ) );
    lines.Push( FVector2D( mBoxPos.X + mBoxSize.X, mBoxPos.Y              ) );
    lines.Push( FVector2D( mBoxPos.X + mBoxSize.X, mBoxPos.Y + mBoxSize.Y ) );
    lines.Push( FVector2D( 0.0f                  , mBoxPos.Y + mBoxSize.Y ) );

	FSlateDrawElement::MakeBox( OutDrawElements
		                      , LayerId
		                      , AllottedGeometry.ToPaintGeometry( mBoxPos, mBoxSize )
		                      , &defaultBrush
		                      , ESlateDrawEffect::None
		                      , fillColor );

	FSlateDrawElement::MakeLines( OutDrawElements
		                        , LayerId
                                , AllottedGeometry.ToPaintGeometry()
		                        , lines
		                        , ESlateDrawEffect::None
		                        , strokeColor
		                        , true
		                        , 2.0f );

    return LayerId;
}

#undef LOCTEXT_NAMESPACE
