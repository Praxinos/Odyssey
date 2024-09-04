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
// From module OdysseyPainterEditor
#include "OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
// From module OdysseyStyle
#include "OdysseyStyleSet.h"
// From module OdysseyVector
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorAnimationCell.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAlter.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::~SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow()
{
}

SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow()
    : mPickedBreakdown( nullptr )
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
    TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> treeView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
    // compute geometry
    FOdysseyAnimationEditorExtension* animationEditorExtension = treeView.Get()->GetAnimationEditorExtension();
    const FVector2D cursorPos = MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() );
    FOdysseyVectorGroupPaint* scene = mInbetweenerTag->GetOwner()->GetScene();
    FOdysseyVectorSharedEnv* sharedEnv = mInbetweenerTag->GetOwner()->GetEngine()->GetSharedEnv();
    std::list<FOdysseyVectorTag*>& tagList = sharedEnv->GetTagList();
    uint64 retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;

    // for AddBreakdown / RemoveBreakdown functions in the context menu
    treeView.Get()->SetCursorPos( cursorPos );

    mPickedBreakdown = nullptr;

    for( FOdysseyVectorTag* tag : tagList )
    {
        tag->GetOwner()->GetEngine()->ClearObjectSelection();
    }

    // This is used by the list view to determine which row is selected.
    mInbetweenerTag->GetOwner()->GetEngine()->SelectObject( mInbetweenerTag->GetOwner() );

/*
    SetItemSelection ( const ItemType& InItem,
bool bSelected,
ESelectInfo::Type SelectInfo
)
*/
    //STableRow<TSharedPtr<FInbetweeningListViewItem>>::OnMouseButtonDown( MyGeometry, MouseEvent );

    if( MouseEvent.IsMouseButtonDown( EKeys::LeftMouseButton ) )
    {
        for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
        {
            uint32 bi = breakdown->GetIndex();

            if( ( cursorPos.X >   mTargetPosBuffer[bi].X )
             && ( cursorPos.Y >   mTargetPosBuffer[bi].Y )
             && ( cursorPos.X < ( mTargetPosBuffer[bi].X + mTargetSizeBuffer[bi].X ) )
             && ( cursorPos.Y < ( mTargetPosBuffer[bi].Y + mTargetSizeBuffer[bi].Y ) ) )
            {
                mPickedBreakdown = breakdown;

                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("vector-timeline-row.transaction.alter","Vector Timeline Alter"));
                if( GUndo )
                {
                    FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownAlter( scene
                                                                                                 , mInbetweenerTag
                                                                                                 , retFlags );

                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                    TSharedPtr<FOdysseyPainterEditorSource> source = animationEditorExtension->GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
                }
                GEditor->EndTransaction();
            }
        }

        return FReply::Handled();
    }

    return FReply::Unhandled();
}


FReply
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::OnMouseMove ( const FGeometry& MyGeometry
                                                                      , const FPointerEvent& MouseEvent )
{
    //STableRow<TSharedPtr<FInbetweeningListViewItem>>::OnMouseMove( MyGeometry, MouseEvent );

    if( MouseEvent.IsMouseButtonDown( EKeys::LeftMouseButton ) )
    {
        if( mPickedBreakdown )
        {
            const TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> listView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
            FOdysseyAnimationEditorExtension* animationEditorExtension = listView.Get()->GetAnimationEditorExtension();
            const FVector2D cursorPos = MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() );
            uint32 frameIndex = animationEditorExtension->Timeline()->GetFrameIndexAtMousePosition( cursorPos.X );
            TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = listView.Get()->GetAnimationLayerImageVector()->GetCellsContainer();
            IOdysseyVectorAnimationCell* tagCell = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell();
            int32 tagCellIndex = tagCell->GetIndex();
            int32 maxCellIndex = tagCell->GetLastCell()->GetIndex();

            if (!cellsContainer)
                return FReply::Unhandled();

            TSharedPtr<FOdysseyAnimationCellImageVector> cell = StaticCastSharedPtr<FOdysseyAnimationCellImageVector>(cellsContainer->GetCellAtFrame(frameIndex));

            if( cell.IsValid() )
            {
                //uint32 inbetweenCount = cell.Get()->GetIndex();
                uint32 cellIndex = cell.Get()->GetIndex();
                uint32 inbetweenCellIndex = cellIndex - tagCellIndex;
                uint32 inbetweenMaxCellIndex = maxCellIndex - tagCellIndex;
                FInbetweenerBreakdown* prevBreakdown = mPickedBreakdown->GetPrevBreakdown();
                FInbetweenerBreakdown* nextBreakdown = mPickedBreakdown->GetNextBreakdown();
                uint32 drawingCount = mInbetweenerTag->GetDrawingCount();
                uint32 prevInbetweenIndex = prevBreakdown ? prevBreakdown->GetTargetDrawingIndex() : 0;
                uint32 nextInbetweenIndex = nextBreakdown ? nextBreakdown->GetTargetDrawingIndex() : inbetweenMaxCellIndex + 1;

                if( ( inbetweenCellIndex > prevInbetweenIndex ) && ( inbetweenCellIndex < nextInbetweenIndex ) )
                {
                    mPickedBreakdown->SetTargetDrawingIndex( inbetweenCellIndex );

                    //MarkPrepassAsDirty();
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
    uint64 retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                    | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                    | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW;

    //STableRow<TSharedPtr<FInbetweeningListViewItem>>::OnMouseButtonUp( MyGeometry, MouseEvent );

    mPickedBreakdown = nullptr;

	if ( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
    {
        mInbetweenerTag->GetOwner()->GetScene()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

        mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->Signal( retFlags );

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FText
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::GetInbetweenerTagInbetweenCount() const
{
    return FText::FromString( FString::FromInt( (int32)mInbetweenerTag->GetDrawingCount() ) );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::CacheDesiredSize ( float LayoutScaleMultiplier )
{
    // retrieve parent widget
    const TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> listView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
    // retrieve timing data
    int32 tagCellIndex = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetIndex();
    IOdysseyVectorAnimationCell* sourceCell = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell();
    uint32 sourceFrame = sourceCell->GetFrame();
    // compute geometry
    FOdysseyAnimationEditorExtension* animationEditorExtension = listView.Get()->GetAnimationEditorExtension();
    float frameWidth = animationEditorExtension->Timeline()->GetFrameWidth();
    uint32 inbetweenCount = ( mInbetweenerTag->GetDrawingCount() - 2 );

    // call from base class
    STableRow<TSharedPtr<FInbetweeningListViewItem>>::CacheDesiredSize( LayoutScaleMultiplier );

    // total size of the widget
    mBoxPos.X  = sourceFrame * frameWidth * LayoutScaleMultiplier;
    mBoxPos.Y  = 0.0f;
    mBoxSize.Y = 20.0f * LayoutScaleMultiplier;

    // source cell has its own rectangle
    mSourcePos.X  = mBoxPos.X;
    mSourcePos.Y  = mBoxPos.Y;
    mSourceSize.X = sourceCell->GetLength() * frameWidth * LayoutScaleMultiplier;
    mSourceSize.Y = mBoxSize.Y;

    mInterpPosBuffer.Reset();
    mInterpPosBuffer.Reserve( inbetweenCount );

    mInterpSizeBuffer.Reset();
    mInterpSizeBuffer.Reserve( inbetweenCount );

    // inbetween cells have their own rectangle
    for( uint32 i = 0; i < inbetweenCount; i++ )
    {
        IOdysseyVectorAnimationCell* interpCell = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetCellByIndex( tagCellIndex + i + 1 );
        uint32 interpFrame = interpCell->GetFrame();

        mInterpPosBuffer.Emplace( interpFrame * frameWidth * LayoutScaleMultiplier, 0.0f );
        mInterpSizeBuffer.Emplace( interpCell->GetLength() * frameWidth * LayoutScaleMultiplier, mBoxSize.Y );
    }

    mTargetPosBuffer.Reset();
    mTargetPosBuffer.Reserve( mInbetweenerTag->GetBreakdownCount() );

    mTargetSizeBuffer.Reset();
    mTargetSizeBuffer.Reserve( mInbetweenerTag->GetBreakdownCount() );

    for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
    {
        IOdysseyVectorAnimationCell* targetCell = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetCellByIndex( tagCellIndex + breakdown->GetTargetDrawingIndex() );
        uint32 targetFrame = targetCell->GetFrame();

        mTargetPosBuffer.Emplace( targetFrame * frameWidth * LayoutScaleMultiplier, 0.0f );
        mTargetSizeBuffer.Emplace( targetCell->GetLength() * frameWidth * LayoutScaleMultiplier, mBoxSize.Y );
    }

    mBoxSize.X = mTargetPosBuffer.Last().X + mTargetSizeBuffer.Last().X;
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
    return mInbetweenerTag->GetOwner()->IsSelected();
}

FCursorReply
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::OnCursorQuery ( const FGeometry& MyGeometry
                                                                        , const FPointerEvent& CursorEvent ) const
{
    const FVector2D cursorPos = MyGeometry.AbsoluteToLocal( CursorEvent.GetScreenSpacePosition() );

    for( int i = 0; ( i < (int)mInbetweenerTag->GetBreakdownCount() )
                    // desired size may not be updated yet, check we are within limits
                 && ( i < (int)mTargetPosBuffer.Num() ); i++ )
    {
        if( ( cursorPos.X >   mTargetPosBuffer[i].X )
         && ( cursorPos.Y >   mTargetPosBuffer[i].Y )
         && ( cursorPos.X < ( mTargetPosBuffer[i].X + mTargetSizeBuffer[i].X ) )
         && ( cursorPos.Y < ( mTargetPosBuffer[i].Y + mTargetSizeBuffer[i].Y ) ) )
        {
            return  FCursorReply::Cursor( EMouseCursor::ResizeLeftRight );
        }
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
    uint32 inbetweenCount = ( mInbetweenerTag->GetDrawingCount() - 2 );
    uint32 breakdownCount = mInbetweenerTag->GetBreakdownCount();
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
    FLinearColor bkdownColor = FLinearColor( 1.0f
                                           , 0.5f
                                           , 0.0f
                                           , 0.25f );
    FLinearColor targetColor = FLinearColor( 0.0f, 0.5f, 0.0f, 0.25f );
    TArray< FVector2D > lines;

	FSlateDrawElement::MakeBox( OutDrawElements
		                      , LayerId
		                      , AllottedGeometry.ToPaintGeometry( mSourcePos, mSourceSize )
		                      , &defaultBrush
		                      , ESlateDrawEffect::None
		                      , sourceColor );

    for( uint32 i = 0; i <  inbetweenCount; i++ )
    {
	    FSlateDrawElement::MakeBox( OutDrawElements
		                            , LayerId
		                            , AllottedGeometry.ToPaintGeometry( mInterpPosBuffer[i]
                                                                      , mInterpSizeBuffer[i] )
		                            , &defaultBrush
		                            , ESlateDrawEffect::None
		                            , interpColor );
    }

    // draw targets (1 per breakdown)
    for( uint32 i = 0; i <  breakdownCount; i++ )
    {
        //if( breakdown->GetTargetInbetweenIndex() != inbetweenCount )
        {
	        FSlateDrawElement::MakeBox( OutDrawElements
		                                , LayerId
		                                , AllottedGeometry.ToPaintGeometry( mTargetPosBuffer[i]
                                                                          , mTargetSizeBuffer[i] )
		                                , &defaultBrush
		                                , ESlateDrawEffect::None
		                                , bkdownColor );
        }
    }
/*
	FSlateDrawElement::MakeBox( OutDrawElements
		                      , LayerId
		                      , AllottedGeometry.ToPaintGeometry( mTargetPos, mTargetSize )
		                      , &defaultBrush
		                      , ESlateDrawEffect::None
		                      , bkdownColor );
*/

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
