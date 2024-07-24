// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

// From module OdysseyAnimationEditor
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
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
    : bHandleClicked( false )
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
/*
    ChildSlot
    [
        SNew( STextBlock)
        .Text( this, &SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::GetInbetweenerTagInbetweenCount )
    ];
*/
}


FReply
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::OnMouseButtonDown( const FGeometry & MyGeometry
                                                                           , const FPointerEvent & MouseEvent )
{
    FOdysseyVectorSharedEnv* sharedEnv = mInbetweenerTag->GetOwner()->GetEngine()->GetSharedEnv();
    std::list<FOdysseyVectorTag*>& tagList = sharedEnv->GetTagList();
    
    for( FOdysseyVectorTag* tag : tagList )
    {
        tag->SetSelected( false );
    }

    mInbetweenerTag->SetSelected( true );

    bHandleClicked = true;

    return FReply::Handled();
}


FReply
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::OnMouseMove ( const FGeometry& MyGeometry
                                                                      , const FPointerEvent& MouseEvent )
{
    if( MouseEvent.IsMouseButtonDown( EKeys::LeftMouseButton ) )
    {
        if( bHandleClicked )
        {
            const TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> listView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
            FOdysseyAnimationEditorExtension* animationEditorExtension = listView.Get()->GetAnimationEditorExtension();
            const FVector2D cursorPos = MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() );
            uint32 frameIndex = animationEditorExtension->Timeline()->GetFrameIndexAtMousePosition( cursorPos.X );
            TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = listView.Get()->GetAnimationLayerImageVector()->GetCellsContainer();
            int32 tagCellIndex = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetIndex();

            if (!cellsContainer)
                return FReply::Unhandled();

            TSharedPtr<FOdysseyAnimationCellImageVector> cell = StaticCastSharedPtr<FOdysseyAnimationCellImageVector>(cellsContainer->GetCellAtFrame(frameIndex));

            if( cell.IsValid() )
            {
                uint32 inbetweenCount = cell.Get()->GetIndex();

                if( cell.Get()->GetIndex() > tagCellIndex )
                {
                    mInbetweenerTag->SetInbetweenCount( cell.Get()->GetIndex() - tagCellIndex - 1 );
                }
            }
        }
    }

    return FReply::Handled();
}

FReply
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::OnMouseButtonUp( const FGeometry & MyGeometry
                                                                         , const FPointerEvent & MouseEvent )
{
    bHandleClicked = false;

    mInbetweenerTag->GetOwner()->GetScene()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return FReply::Handled();
}

FText
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::GetInbetweenerTagInbetweenCount() const
{
    return FText::FromString( FString::FromInt( (int32)mInbetweenerTag->GetInbetweenCount() ) );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::CacheDesiredSize ( float LayoutScaleMultiplier )
{
    // retrieve parent widget
    const TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> listView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
    // retrieve timing data
    int32 tagCellIndex = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetIndex();
    int32 tagCellCount = mInbetweenerTag->GetInbetweenCount();
    IOdysseyVectorAnimationCell* sourceCell = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell();
    IOdysseyVectorAnimationCell* targetCell = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetCellByIndex( tagCellIndex + tagCellCount + 1 );
    uint32 sourceFrame = sourceCell->GetFrame();
    uint32 targetFrame = targetCell->GetFrame();
    // compute geometry
    FOdysseyAnimationEditorExtension* animationEditorExtension = listView.Get()->GetAnimationEditorExtension();
    float frameWidth = animationEditorExtension->Timeline()->GetFrameWidth();

    // call from base class
    STableRow<TSharedPtr<FInbetweeningListViewItem>>::CacheDesiredSize( LayoutScaleMultiplier );

    // total size of the widget
    mBoxPos.X  = sourceFrame * frameWidth * LayoutScaleMultiplier;
    mBoxPos.Y  = 0.0f;
    mBoxSize.X = ( targetFrame + targetCell->GetLength() ) * frameWidth * LayoutScaleMultiplier;
    mBoxSize.Y = 20.0f * LayoutScaleMultiplier;

    // source cell has its own rectangle
    mSourcePos.X  = mBoxPos.X;
    mSourcePos.Y  = mBoxPos.Y;
    mSourceSize.X = sourceCell->GetLength() * frameWidth * LayoutScaleMultiplier;
    mSourceSize.Y = mBoxSize.Y;

    mInterpPosBuffer.Reset();
    mInterpPosBuffer.Reserve( mInbetweenerTag->GetInbetweenCount() );

    mInterpSizeBuffer.Reset();
    mInterpSizeBuffer.Reserve( mInbetweenerTag->GetInbetweenCount() );

    // inbetween cells have their own rectangle
    for( uint32 i = 0; i <  mInbetweenerTag->GetInbetweenCount(); i++ )
    {
        IOdysseyVectorAnimationCell* interpCell = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetCellByIndex( tagCellIndex + i + 1 );
        uint32 interpFrame = interpCell->GetFrame();

        mInterpPosBuffer.Emplace( interpFrame * frameWidth * LayoutScaleMultiplier, 0.0f );
        mInterpSizeBuffer.Emplace( interpCell->GetLength() * frameWidth * LayoutScaleMultiplier, mBoxSize.Y );
    }

    // target cell has its own rectangle
    mTargetPos.X  = targetFrame * frameWidth * LayoutScaleMultiplier;
    mTargetPos.Y  = 0.0f;
    mTargetSize.X = targetCell->GetLength() * frameWidth * LayoutScaleMultiplier;
    mTargetSize.Y = mBoxSize.Y;
}

FVector2D
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::ComputeDesiredSize ( float LayoutScaleMultiplier ) const
{
    // Note: dimensions are already scaled in CacheDesiredSize()
    return FVector2D( mBoxSize.X, mBoxSize.Y );
}

bool  	
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::IsItemSelected() const
{
    return mInbetweenerTag->IsSelected();
}

FCursorReply
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::OnCursorQuery ( const FGeometry& MyGeometry
                                                                        , const FPointerEvent& CursorEvent ) const
{
    const FVector2D cursorPos = MyGeometry.AbsoluteToLocal( CursorEvent.GetScreenSpacePosition() );

    if( ( cursorPos.X > mTargetPos.X )
     && ( cursorPos.Y > mTargetPos.Y )
     && ( cursorPos.X < ( mTargetPos.X + mTargetSize.X ) )
     && ( cursorPos.Y < ( mTargetPos.Y + mTargetSize.Y ) ) )
    {
        return  FCursorReply::Cursor( EMouseCursor::ResizeLeftRight );
    }

    return FCursorReply::Unhandled();
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

	LayerId = STableRow<TSharedPtr<FInbetweeningListViewItem>>::OnPaint( Args
                                                                       , AllottedGeometry
                                                                       , MyCullingRect
                                                                       , OutDrawElements
                                                                       , LayerId
                                                                       , InWidgetStyle
                                                                       , bParentEnabled );
	++LayerId;
    FLinearColor strokeColor = FLinearColor( 0.5f, 0.5f, 0.5f, 0.5f  );
    FLinearColor sourceColor = FLinearColor( 0.5f, 0.5f, 0.5f, 0.25f );
    FLinearColor interpColor = FLinearColor( inbetweenerTagColor.R
                                           , inbetweenerTagColor.G
                                           , inbetweenerTagColor.B
                                           , 0.25f );
    FLinearColor targetColor = FLinearColor( 0.0f, 0.5f, 0.0f, 0.25f );
    TArray< FVector2D > lines;

	FSlateDrawElement::MakeBox( OutDrawElements
		                      , LayerId
		                      , AllottedGeometry.ToPaintGeometry( mSourcePos, mSourceSize )
		                      , &defaultBrush
		                      , ESlateDrawEffect::None
		                      , sourceColor );

    for( uint32 i = 0; i <  mInbetweenerTag->GetInbetweenCount(); i++ )
    {
	    FSlateDrawElement::MakeBox( OutDrawElements
		                          , LayerId
		                          , AllottedGeometry.ToPaintGeometry( mInterpPosBuffer[i], mInterpSizeBuffer[i] )
		                          , &defaultBrush
		                          , ESlateDrawEffect::None
		                          , interpColor );
    }

	FSlateDrawElement::MakeBox( OutDrawElements
		                      , LayerId
		                      , AllottedGeometry.ToPaintGeometry( mTargetPos, mTargetSize )
		                      , &defaultBrush
		                      , ESlateDrawEffect::None
		                      , targetColor );

    lines.Reserve( 5 );
    lines.Push( FVector2D( mBoxPos.X             , mBoxPos.Y              ) );
    lines.Push( FVector2D( mBoxPos.X + mBoxSize.X, mBoxPos.Y              ) );
    lines.Push( FVector2D( mBoxPos.X + mBoxSize.X, mBoxPos.Y + mBoxSize.Y ) );
    lines.Push( FVector2D( 0.0f                  , mBoxPos.Y + mBoxSize.Y ) );

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
