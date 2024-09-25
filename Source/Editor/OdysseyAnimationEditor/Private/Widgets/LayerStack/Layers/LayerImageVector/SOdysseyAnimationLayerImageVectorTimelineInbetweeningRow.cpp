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
// From module OdysseyAnimation
#include "OdysseyAnimation.h"
// From module OdysseyLayerStack
#include "OdysseyLayerStack.h"
// From module OdysseyPainterEditor
#include "OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
// From module OdysseyStyle
#include "OdysseyStyleSet.h"
// From module OdysseyVector
#include "OdysseyVectorObject.h"
#include "OdysseyVectorRoot.h"
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
    , mCandidateTargetCellBox( EInbetweeningRowCellBoxType::None, 0, 0.0f, 0.0f, 0.0f, 0.0f )
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
    FOdysseyAnimationEditorExtension* animationEditorExtension = treeView.Get()->GetAnimationEditorExtension();
    const FVector2D cursorPos = MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() );
    UOdysseyAnimationLayerImageVector* layer = treeView.Get()->GetAnimationLayerImageVector();
    UOdysseyLayerStack* layerStack = animationEditorExtension->GetEditor()->LayerStack();
    FOdysseyVectorSharedEnv* sharedEnv = mInbetweenerTag->GetOwner()->GetSharedEnv();
    std::list<FOdysseyVectorTag*>& sharedTagList = sharedEnv->GetSharedTagList();
    FOdysseyVectorGroupPaint* scene = mInbetweenerTag->GetOwner()->GetScene();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;
    FReply reply = FReply::Unhandled();

    // for AddBreakdown / RemoveBreakdown functions in the context menu
    treeView.Get()->SetCursorPos( cursorPos );

    if ( layerStack->CurrentLayer.Get() != layer )
    {
        FOdysseyObjectEditorUtils::SetPropertyValue( layerStack
                                                    , "CurrentLayer"
                                                    , TSoftObjectPtr<UOdysseyLayer>( layer ) );
    }

    mPickedBreakdown = nullptr;
    mCandidateTargetCellBox.type = EInbetweeningRowCellBoxType::None;

    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
    {
        for( FOdysseyVectorTag* tag : sharedTagList )
        {
            tag->GetOwner()->GetEngine()->UnselectObject( tag->GetOwner() );
        }
    }

    // This is used by the list view to determine which row is selected.
    mInbetweenerTag->GetOwner()->GetEngine()->SelectObject( mInbetweenerTag->GetOwner() );

    if( MouseEvent.IsMouseButtonDown( EKeys::LeftMouseButton ) )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-timeline-row.transaction.alter","Vector Timeline Alter"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownAlter( scene
                                                                                         , mInbetweenerTag
                                                                                         , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = animationEditorExtension->GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
        {
            uint32 bi = breakdown->GetTargetDrawingIndex();

            if( bi < (uint32) mCellBoxBuffer.Num() )
            {
                if( ( cursorPos.X >   mCellBoxBuffer[bi].x )
                 && ( cursorPos.Y >   mCellBoxBuffer[bi].y )
                 && ( cursorPos.X < ( mCellBoxBuffer[bi].x + mCellBoxBuffer[bi].w ) )
                 && ( cursorPos.Y < ( mCellBoxBuffer[bi].y + mCellBoxBuffer[bi].h ) ) )
                {
                    mPickedBreakdown = breakdown;
                }
            }
        }

        reply = FReply::Handled();
    }

    // request redraw
    mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->Invalidate( 0 );
    // update UI
    FOdysseyVectorEngine::Notify( nullptr, notificationFlags );

    return reply;
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
            float frameWidth = animationEditorExtension->Timeline()->GetFrameWidth();

            if (!cellsContainer)
                return FReply::Unhandled();

            TSharedPtr<FOdysseyAnimationCellImageVector> cursorCell = StaticCastSharedPtr<FOdysseyAnimationCellImageVector>(cellsContainer->GetCellAtFrame(frameIndex));

            if( cursorCell.IsValid() )
            {
                uint32 cursorCellIndex = cursorCell.Get()->GetIndex();
                uint32 tagCellIndex = tagCell->GetIndex();
                int32 drawingIndex = ( mInbetweenerTag->GetInterpolationDirection() == eInbetweenerInterpolationDirection::Forward ) ? (int32)( cursorCellIndex - tagCellIndex ) 
                                                                                                                                      : (int32)( tagCellIndex - cursorCellIndex );
                FInbetweenerBreakdown* prevBreakdown = mPickedBreakdown->GetPrevBreakdown();
                FInbetweenerBreakdown* nextBreakdown = mPickedBreakdown->GetNextBreakdown();

                 // reverse the whole thing now to ease the calculations
                if( cursorCellIndex < tagCellIndex )
                {
                    if( mInbetweenerTag->GetBreakdownCount() == 1 )
                    {
                        mInbetweenerTag->InvertInterpolationDirection();

                        mInbetweenerTag->GetOwner()->GetScene()->Update( 0 );

                        CacheDesiredSize( mLayoutScaleMultiplier );

                        drawingIndex = -drawingIndex;
                    }
                    else
                    {
                        drawingIndex = mPickedBreakdown->GetSourceDrawingIndex();
                    }
                }

                uint32 maxCellIndex = ( mInbetweenerTag->GetInterpolationDirection() == eInbetweenerInterpolationDirection::Forward ) ? tagCell->GetLastCell()->GetIndex()
                                                                                                                                      : tagCell->GetFirstCell()->GetIndex();
                uint32 maxDrawingIndex = ( maxCellIndex - cursorCellIndex );
                uint32 prevDrawingIndex = prevBreakdown ? prevBreakdown->GetTargetDrawingIndex() : 0;
                uint32 nextDrawingIndex = nextBreakdown ? nextBreakdown->GetTargetDrawingIndex() : maxDrawingIndex + 1;

                if( ( (uint32)drawingIndex > prevDrawingIndex ) && ( (uint32)drawingIndex < nextDrawingIndex ) )
                {
                    mCandidateTargetCellBox = FInbetweeningRowCellBox( EInbetweeningRowCellBoxType::Target
                                                                     , drawingIndex
                                                                     , cursorCell->GetFrame() * frameWidth * mLayoutScaleMultiplier
                                                                     , 0.0f
                                                                     , cursorCell->GetLength() * frameWidth * mLayoutScaleMultiplier
                                                                     , mBoxSize.Y );

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
    TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> treeView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
    FOdysseyAnimationEditorExtension* animationEditorExtension = treeView.Get()->GetAnimationEditorExtension();
    int currentFrame = animationEditorExtension->Animation()->CurrentFrame;
    UOdysseyAnimationLayerImageVector* vectorLayer = treeView.Get()->GetAnimationLayerImageVector();
    TSharedPtr<FOdysseyAnimationCell> cell = vectorLayer->GetCellsContainer()->GetCellAtFrame( currentFrame );
    FReply reply = FReply::Unhandled();

    if( cell.IsValid() )
    {
        FOdysseyAnimationCellImageVector* vectorCell = static_cast<FOdysseyAnimationCellImageVector*>(cell.Get());
        uint64 retFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                        | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                        | FOdysseyPainterEditor::UI_UPDATE_HUD;

        //STableRow<TSharedPtr<FInbetweeningListViewItem>>::OnMouseButtonUp( MyGeometry, MouseEvent );

	    if ( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
        {
            if( mPickedBreakdown )
            {
                if( mCandidateTargetCellBox.type == EInbetweeningRowCellBoxType::Target )
                {
                    mPickedBreakdown->SetTargetDrawingIndex( mCandidateTargetCellBox.index );
                }
            }

            mInbetweenerTag->GetOwner()->GetScene()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
            // request for redrawing of the current displayed cell
            vectorCell->GetRoot()->GetEngine()->Invalidate( 0 );
            FOdysseyVectorEngine::Notify( nullptr, retFlags );

            reply = FReply::Handled();
        }
    }

    mPickedBreakdown = nullptr;
    mCandidateTargetCellBox.type = EInbetweeningRowCellBoxType::None;

    return reply;
}

FText
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::GetInbetweenerTagInbetweenCount() const
{
    return FText::FromString( FString::FromInt( (int32)mInbetweenerTag->GetLength() ) );
}

void
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::CacheDesiredSize ( float LayoutScaleMultiplier )
{
    // retrieve parent widget
    const TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> listView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
    // retrieve timing data
    uint32 tagCellIndex = mInbetweenerTag->GetAnimationCellIndex();
    IOdysseyVectorAnimationCell* sourceCell = mInbetweenerTag->GetAnimationCell();
    uint32 sourceFrame = sourceCell->GetFrame();
    // compute geometry
    FOdysseyAnimationEditorExtension* animationEditorExtension = listView.Get()->GetAnimationEditorExtension();
    float frameWidth = animationEditorExtension->Timeline()->GetFrameWidth();
    FOdysseyVectorEngine* vectorEngine = mInbetweenerTag->GetOwner()->GetScene()->GetEngine();
    double xmin, xmax;

    // call from base class
    STableRow<TSharedPtr<FInbetweeningListViewItem>>::CacheDesiredSize( LayoutScaleMultiplier );

    mLayoutScaleMultiplier = LayoutScaleMultiplier;

    // total vertical pos & size of the widget
    mBoxPos.Y  = 0.0f;
    mBoxSize.Y = 20.0f * LayoutScaleMultiplier;

    mCellBoxBuffer.Reset();
    mCellBoxBuffer.Reserve( mInbetweenerTag->GetLength() );

    mCellBoxBuffer.Emplace( EInbetweeningRowCellBoxType::Source
                          , 0
                          , sourceFrame * frameWidth * LayoutScaleMultiplier
                          , 0.0f
                          , sourceCell->GetLength() * frameWidth * LayoutScaleMultiplier
                          , mBoxSize.Y );

    for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
    {
        uint32 sourceIndex = breakdown->GetSourceDrawingIndex();
        uint32 targetIndex = breakdown->GetTargetDrawingIndex();
        uint32 targetCellIndex = breakdown->GetTargetAnimationCellIndex();
        IOdysseyVectorAnimationCell* targetCell = vectorEngine->GetAnimationCell()->GetCellByIndex( targetCellIndex );

        // targetCell can be NULL if there is no further cell
        if( targetCell )
        {
            uint32 targetFrame = targetCell->GetFrame();

            for( uint32 i = sourceIndex + 1; i < targetIndex; i++ )
            {
                FInbetweenerDrawing* drawing = mInbetweenerTag->GetDrawing( i );
                int32 inbetweenCellIndex = drawing->GetAnimationCellIndex();
                IOdysseyVectorAnimationCell* inbetweenCell = vectorEngine->GetAnimationCell()->GetCellByIndex(inbetweenCellIndex);

                if( inbetweenCell )
                {
                    uint32 inbetweenFrame = inbetweenCell->GetFrame();

                    mCellBoxBuffer.Emplace( EInbetweeningRowCellBoxType::Inbetween
                                          , i
                                          , inbetweenFrame * frameWidth * LayoutScaleMultiplier
                                          , 0.0f
                                          , inbetweenCell->GetLength() * frameWidth * LayoutScaleMultiplier
                                          , mBoxSize.Y );
                }
            }

            mCellBoxBuffer.Emplace( EInbetweeningRowCellBoxType::Target
                                  , targetIndex
                                  , targetFrame * frameWidth * LayoutScaleMultiplier
                                  , 0.0f
                                  , targetCell->GetLength() * frameWidth * LayoutScaleMultiplier
                                  , mBoxSize.Y );
        }
    }

    // the whole bounding box
    xmin = xmax = mBoxPos.X;

    for( int32 i = 0; i < mCellBoxBuffer.Num(); i++ )
    {
        double x1 = mCellBoxBuffer[i].x;
        double x2 = mCellBoxBuffer[i].x + mCellBoxBuffer[i].w - 1;

        xmin = std::min( x1, xmin );
        xmax = std::max( x2, xmax );
    }

    // total horizontal pos & size of the widget
    mBoxPos.X = xmin;
    mBoxSize.X = xmax - xmin + 1;
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

    for( int i = 0; i < (int) mCellBoxBuffer.Num(); i++ )
    {
        if( mCellBoxBuffer[i].type == EInbetweeningRowCellBoxType::Target )
        {
            if( ( cursorPos.X >   mCellBoxBuffer[i].x )
             && ( cursorPos.Y >   mCellBoxBuffer[i].y )
             && ( cursorPos.X < ( mCellBoxBuffer[i].x + mCellBoxBuffer[i].w ) )
             && ( cursorPos.Y < ( mCellBoxBuffer[i].y + mCellBoxBuffer[i].h ) ) )
            {
                return  FCursorReply::Cursor( EMouseCursor::ResizeLeftRight );
            }
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
    TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> treeView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
    // compute geometry
    FOdysseyAnimationEditorExtension* animationEditorExtension = treeView.Get()->GetAnimationEditorExtension();
    uint32 inbetweenCount = ( mInbetweenerTag->GetLength() - 2 );
    uint32 breakdownCount = mInbetweenerTag->GetBreakdownCount();
    const FColor& inbetweenerTagColor = mInbetweenerTag->GetColor();
    // offset in "number of frames"
	float offset = animationEditorExtension->Timeline()->GetOffset();
	float frameSize = animationEditorExtension->Timeline()->GetFrameWidth();
    float scrollByPixels = offset * frameSize * mLayoutScaleMultiplier;
    static FSlateBrush defaultBrush;

	LayerId = STableRow<TSharedPtr<FInbetweeningListViewItem>>::OnPaint( Args
                                                                       , AllottedGeometry
                                                                       , MyCullingRect
                                                                       , OutDrawElements
                                                                       , LayerId
                                                                       , InWidgetStyle
                                                                       , bParentEnabled );
	++LayerId;
    FLinearColor strokeColor = FLinearColor( 0.5f, 0.5f, 0.5f, 0.5f );
    FLinearColor sourceColor = FLinearColor( 0.5f, 0.5f, 0.5f, 0.5f );
    FLinearColor interpColor = FLinearColor( 0.25f
                                           , 0.25f
                                           , 0.25f
                                           , 0.25f );
    FLinearColor targetColor = FLinearColor( 1.0f, 0.5f, 0.5f, 0.5f );
    TArray< FVector2D > lines;

    for( int i = 0; ( i < (int) mCellBoxBuffer.Num() ); i++ )
    {
        FLinearColor color = FLinearColor( 0.0f, 0.0f, 0.0f, 0.0f );

        switch ( mCellBoxBuffer[i].type )
        {
            case EInbetweeningRowCellBoxType::Source :
                color = sourceColor;
            break;

            case EInbetweeningRowCellBoxType::Inbetween :
                color = interpColor;
            break;

            case EInbetweeningRowCellBoxType::Target :
                color = targetColor;
            break;

            default :
            break;
        }

	    FSlateDrawElement::MakeBox( OutDrawElements
		                          , LayerId
		                          , AllottedGeometry.ToPaintGeometry( FVector2D( mCellBoxBuffer[i].x - ( scrollByPixels )
                                                                               , mCellBoxBuffer[i].y )
                                                                    , FVector2D( mCellBoxBuffer[i].w
                                                                               , mCellBoxBuffer[i].h ) )
		                          , &defaultBrush
		                          , ESlateDrawEffect::None
		                          , color );

        if( mCellBoxBuffer[i].type == EInbetweeningRowCellBoxType::Source )
        {
            // draw forward arrow
            if( mInbetweenerTag->GetInterpolationDirection() == eInbetweenerInterpolationDirection::Forward )
            {
	            FSlateDrawElement::MakeBox( OutDrawElements
		                                  , LayerId
		                                  , AllottedGeometry.ToPaintGeometry( FVector2D( mCellBoxBuffer[i].x - ( scrollByPixels )
                                                                                       , mCellBoxBuffer[i].y )
                                                                            , FVector2D( 16
                                                                                       , mCellBoxBuffer[i].h ) )
		                                  , treeView.Get()->GetForwardArrowBrush()
		                                  , ESlateDrawEffect::None
		                                  , color );
            }

            // draw backward arrow
            if( mInbetweenerTag->GetInterpolationDirection() == eInbetweenerInterpolationDirection::Backward )
            {
	            FSlateDrawElement::MakeBox( OutDrawElements
		                                  , LayerId
		                                  , AllottedGeometry.ToPaintGeometry( FVector2D( mCellBoxBuffer[i].x + mCellBoxBuffer[i].w - 16  - ( scrollByPixels )
                                                                                       , mCellBoxBuffer[i].y )
                                                                            , FVector2D( 16
                                                                                       , mCellBoxBuffer[i].h ) )
		                                  , treeView.Get()->GetBackwardArrowBrush()
		                                  , ESlateDrawEffect::None
		                                  , color );
            }
        }
    }

    if( mCandidateTargetCellBox.type == EInbetweeningRowCellBoxType::Target )
    {
        // draw candidate target (when resizing breakdown)
	    FSlateDrawElement::MakeBox( OutDrawElements
		                          , LayerId
		                          , AllottedGeometry.ToPaintGeometry( FVector2D( mCandidateTargetCellBox.x - ( scrollByPixels )
                                                                               , mCandidateTargetCellBox.y )
                                                                    , FVector2D( mCandidateTargetCellBox.w
                                                                               , mCandidateTargetCellBox.h ) )
		                          , &defaultBrush
		                          , ESlateDrawEffect::None
		                          , targetColor );
    }

    lines.Reserve( 4 );
    lines.Push( FVector2D( 0.0f      , 0.0f       ) );
    lines.Push( FVector2D( mBoxSize.X, 0.0f       ) );
    lines.Push( FVector2D( mBoxSize.X, mBoxSize.Y ) );
    lines.Push( FVector2D( 0.0f      , mBoxSize.Y ) );

	FSlateDrawElement::MakeLines( OutDrawElements
		                        , LayerId
		                        , AllottedGeometry.ToPaintGeometry( FVector2D( mBoxPos.X - ( scrollByPixels )
                                                                             , mBoxPos.Y )
                                                                  , FVector2D( mBoxSize.X
                                                                             , mBoxSize.Y ) )
		                        , lines
		                        , ESlateDrawEffect::None
		                        , strokeColor
		                        , true
		                        , 2.0f );

    return LayerId;
}

#undef LOCTEXT_NAMESPACE
