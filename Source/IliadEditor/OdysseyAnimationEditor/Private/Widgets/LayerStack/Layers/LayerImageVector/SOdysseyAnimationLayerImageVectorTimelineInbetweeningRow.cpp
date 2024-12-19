// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

// From module OdysseyAnimationEditor
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
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
#include "OdysseyVectorCell.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAlter.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyAnimationEditorTimelinePosition.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::~SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow()
{
}

SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow()
    : mPickedBreakdown( nullptr )
    , mCandidateTargetCellBox( 0, 0, 0.0f, 0.0f, 0.0f, 0.0f )
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
    const FVector2D cursorPos = MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() );
    UOdysseyAnimationLayerImageVector* layer = treeView.Get()->GetAnimationLayerImageVector();
    FOdysseyPainterEditor* editor = treeView->GetEditor();
    UOdysseyLayerStack* layerStack = layer->GetLayerStack();
    FOdysseyVectorObject* ownerObject = mInbetweenerTag->GetOwner();
    FOdysseyVectorEngine* vectorEngine = ownerObject->GetEngine();
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
    mCandidateTargetCellBox.type = 0;

    // Note: we don't rely on STreeView::SelectedItems to keep track of the selection.
    // That way we don't have to update the widget.
    // We directly rely on the selection from our vector engine. However this implies
    // that we have to deal with the multiple selection by ourselves.

    if( FSlateApplication::Get().GetModifierKeys().IsShiftDown() == true )
    {
        FOdysseyVectorObject* lastSelectedObject = vectorEngine->GetLastSelectedObject();

        if( lastSelectedObject )
        {
            bool doSelect = false;

            for( const TSharedPtr<FInbetweeningListViewItem>& item : treeView.Get()->GetItems() )
            {
                FOdysseyVectorObject* itemObject = item.Get()->GetInbetweenerTag()->GetOwner();

                if( ( itemObject == ownerObject ) || ( itemObject == lastSelectedObject ) )
                {
                    doSelect = !doSelect;
                }

                if( doSelect )
                {
                    if( itemObject->IsSelected() == false )
                    {
                        vectorEngine->SelectObject( itemObject );
                    }
                }
            }
        }
    }
    else
    {
        if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
        {
            for( FOdysseyVectorObject* rootObject : sharedEnv->GetChildrenList() )
            {
                rootObject->GetEngine()->ClearObjectSelection();
            }
        }
    }

    if( ownerObject->IsSelected() == false )
    {
        vectorEngine->SelectObject( ownerObject );
    }
    else
    {
        if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
        {
            vectorEngine->UnselectObject( ownerObject );
        }
    }

    if( MouseEvent.IsMouseButtonDown( EKeys::LeftMouseButton ) )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-timeline-row.transaction.alter","Vector Timeline Alter"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownAlter( sharedEnv
                                                                                         , mInbetweenerTag
                                                                                         , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            if (editor)
            {
                TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
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

        reply = FReply::Handled().CaptureMouse( AsShared() );
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
    TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> treeView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
    TSharedPtr<FOdysseyAnimationEditorTimelinePosition> timelinePosition = treeView->GetTimelinePosition();

    if( MouseEvent.IsMouseButtonDown( EKeys::LeftMouseButton ) )
    {
        if( mPickedBreakdown )
        {
            const TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening> listView = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>(OwnerTablePtr.Pin());
            const FVector2D cursorPos = MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() );
            uint32 frameIndex = timelinePosition->MousePositionToFrame( cursorPos.X );
            IOdysseyVectorCell* tagCell = mInbetweenerTag->GetOwner()->GetEngine()->GetCell();
            float frameWidth = timelinePosition->GetFrameSize();

            UOdysseyAnimationCell* cell = listView.Get()->GetAnimationLayerImageVector()->GetCellAtFrame(frameIndex);
            if (!cell || !cell->IsA<UOdysseyAnimationCellImageVector>())
                return FReply::Unhandled();

            UOdysseyAnimationCellImageVector* cursorCell = Cast<UOdysseyAnimationCellImageVector>(cell);
            if( cursorCell )
            {
                uint32 cursorCellIndex = cursorCell->GetIndex();
                uint32 tagCellIndex = tagCell->GetIndex();
                uint32 drawingIndex = abs( (int)(cursorCellIndex - tagCellIndex) );
                FInbetweenerBreakdown* prevBreakdown = mPickedBreakdown->GetPrevBreakdown();
                FInbetweenerBreakdown* nextBreakdown = mPickedBreakdown->GetNextBreakdown();

                 // reverse the whole thing now to ease the calculations
                if( mInbetweenerTag->GetBreakdownCount() == 1 )
                {
                    eInbetweenerInterpolationDirection direction = ( cursorCellIndex < tagCellIndex ) ? eInbetweenerInterpolationDirection::Backward
                                                                                                      : eInbetweenerInterpolationDirection::Forward;

                    if( mInbetweenerTag->GetInterpolationDirection() != direction )
                    {
                        mInbetweenerTag->SetInterpolationDirection( direction );

                        mInbetweenerTag->GetOwner()->GetScene()->Update( 0 );

                        CacheDesiredSize( 1.0f /* mLayoutScaleMultiplier */);
                    }
                }

                uint32 maxCellIndex = ( mInbetweenerTag->GetInterpolationDirection() == eInbetweenerInterpolationDirection::Forward ) ? listView->GetAnimationLayerImageVector()->GetLastCell()->GetIndex()
                                                                                                                                      : listView->GetAnimationLayerImageVector()->GetFirstCell()->GetIndex();
                uint32 maxDrawingIndex = abs( (int)(maxCellIndex - tagCellIndex) );
                uint32 prevDrawingIndex = prevBreakdown ? prevBreakdown->GetTargetDrawingIndex() : 0;
                uint32 nextDrawingIndex = nextBreakdown ? nextBreakdown->GetTargetDrawingIndex() : maxDrawingIndex + 1;

                if( ( (uint32)drawingIndex > prevDrawingIndex ) && ( (uint32)drawingIndex < nextDrawingIndex ) )
                {
                    mCandidateTargetCellBox = CellBox( CellBox::TYPE_TARGET
                                                                     , drawingIndex
                                                                     , cursorCell->GetFrame() * frameWidth /* * mLayoutScaleMultiplier */
                                                                     , 0.0f
                                                                     , cursorCell->GetLength() * frameWidth /* * mLayoutScaleMultiplier */
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
    UOdysseyAnimationLayerImageVector* vectorLayer = treeView.Get()->GetAnimationLayerImageVector();
    int currentFrame = vectorLayer->GetAnimation()->CurrentFrame;
    UOdysseyAnimationCell* cell = vectorLayer->GetCellAtFrame( currentFrame );
    FReply reply = FReply::Unhandled();

    if( cell && cell->IsA<UOdysseyAnimationCellImageVector>())
    {
        UOdysseyAnimationCellImageVector* vectorCell = Cast<UOdysseyAnimationCellImageVector>(cell);
        uint64 retFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                        | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                        | FOdysseyPainterEditor::UI_UPDATE_HUD;

        //STableRow<TSharedPtr<FInbetweeningListViewItem>>::OnMouseButtonUp( MyGeometry, MouseEvent );

        if ( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
        {
            if( mPickedBreakdown )
            {
                if( mCandidateTargetCellBox.type & CellBox::TYPE_TARGET )
                {
                    mPickedBreakdown->SetTargetDrawingIndex( mCandidateTargetCellBox.index );
                }
            }

            mInbetweenerTag->GetOwner()->GetScene()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
            // request for redrawing of the current displayed cell
            vectorCell->GetRoot()->GetEngine()->Invalidate( 0 );
            FOdysseyVectorEngine::Notify( nullptr, retFlags );

            reply = FReply::Handled().ReleaseMouseCapture();
        }
    }

    mPickedBreakdown = nullptr;
    mCandidateTargetCellBox.type = 0;

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
    uint32 tagCellIndex = mInbetweenerTag->GetSourceCellIndex();
    IOdysseyVectorCell* sourceCell = mInbetweenerTag->GetCell();
    uint32 sourceFrame = sourceCell->GetFrame();
    // compute geometry
    float frameWidth = listView->GetTimelinePosition()->GetFrameSize();
    FOdysseyVectorEngine* vectorEngine = mInbetweenerTag->GetOwner()->GetScene()->GetEngine();
    IOdysseyVectorLayer* layer = vectorEngine->GetLayer();
    double xmin, xmax;

    // call from base class
    STableRow<TSharedPtr<FInbetweeningListViewItem>>::CacheDesiredSize( LayoutScaleMultiplier );

    //mLayoutScaleMultiplier = LayoutScaleMultiplier;

    // total vertical pos & size of the widget
    mBoxPos.Y  = 0.0f;
    mBoxSize.Y = 20.0f /* * LayoutScaleMultiplier */;

    mCellBoxBuffer.Reset();
    mCellBoxBuffer.Reserve( mInbetweenerTag->GetLength() );

    mCellBoxBuffer.Emplace( CellBox::TYPE_SOURCE
                          , 0
                          , sourceFrame * frameWidth /* * LayoutScaleMultiplier */
                          , 0.0f
                          , sourceCell->GetLength() * frameWidth /* * LayoutScaleMultiplier */
                          , mBoxSize.Y );

    for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
    {
        uint32 sourceIndex = breakdown->GetSourceDrawingIndex();
        uint32 targetIndex = breakdown->GetTargetDrawingIndex();
        uint32 targetCellIndex = breakdown->GetTargetCellIndex();
        IOdysseyVectorCell* targetCell = layer->GetCellByIndex( targetCellIndex );

        // targetCell can be NULL if there is no further cell
        if( targetCell )
        {
            uint32 targetFrame = targetCell->GetFrame();

            for( uint32 i = sourceIndex + 1; i < targetIndex; i++ )
            {
                FInbetweenerDrawing* drawing = mInbetweenerTag->GetDrawing( i );
                int32 inbetweenCellIndex = drawing->GetCellIndex();
                IOdysseyVectorCell* inbetweenCell = layer->GetCellByIndex( inbetweenCellIndex );

                if( inbetweenCell )
                {
                    uint32 inbetweenFrame = inbetweenCell->GetFrame();

                    mCellBoxBuffer.Emplace( CellBox::TYPE_INBETWEEN
                                          , i
                                          , inbetweenFrame * frameWidth /* * LayoutScaleMultiplier */
                                          , 0.0f
                                          , inbetweenCell->GetLength() * frameWidth /* * LayoutScaleMultiplier */
                                          , mBoxSize.Y );
                }
            }

            mCellBoxBuffer.Emplace( ( breakdown->GetNextBreakdown() ? ( CellBox::TYPE_TARGET
                                                                      | CellBox::TYPE_SOURCE )
                                                                    : CellBox::TYPE_TARGET )
                                  | ( breakdown->IsTargetVisible()  ? CellBox::TYPE_VISIBLE : 0 )
                                  , targetIndex
                                  , targetFrame * frameWidth /* * LayoutScaleMultiplier */
                                  , 0.0f
                                  , targetCell->GetLength() * frameWidth /* * LayoutScaleMultiplier */
                                  , mBoxSize.Y );
        }
    }

    // the whole bounding box
    xmin =  DBL_MAX;
    xmax = -DBL_MAX;

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

FCursorReply
SOdysseyAnimationLayerImageVectorTimelineInbetweeningRow::OnCursorQuery ( const FGeometry& MyGeometry
                                                                        , const FPointerEvent& CursorEvent ) const
{
    const FVector2D cursorPos = MyGeometry.AbsoluteToLocal( CursorEvent.GetScreenSpacePosition() );

    for( int i = 0; i < (int) mCellBoxBuffer.Num(); i++ )
    {
        if( mCellBoxBuffer[i].type & CellBox::TYPE_TARGET )
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
    uint32 inbetweenCount = ( mInbetweenerTag->GetLength() - 2 );
    uint32 breakdownCount = mInbetweenerTag->GetBreakdownCount();
    const FColor& inbetweenerTagColor = mInbetweenerTag->GetInbetweenColor();
    // offset in "number of frames"

    TSharedPtr<FOdysseyAnimationEditorTimelinePosition> timelinePosition = treeView->GetTimelinePosition();
    float offset = timelinePosition->GetOffset();
    float frameSize = timelinePosition->GetFrameSize();
    //float scrollByPixels = offset * frameSize /* * mLayoutScaleMultiplier */;
    static FSlateBrush defaultBrush;
    const FSlateBrush* visibilityBrush = FOdysseyStyle::GetBrush( TEXT( "OdysseyLayerStack.Visible16" ) );

    LayerId = STableRow<TSharedPtr<FInbetweeningListViewItem>>::OnPaint( Args
                                                                       , AllottedGeometry
                                                                       , MyCullingRect
                                                                       , OutDrawElements
                                                                       , LayerId
                                                                       , InWidgetStyle
                                                                       , bParentEnabled );
    ++LayerId;
    // declared static to save some CPU cycles as there is no need to initialize them at each all
    static const FLinearColor strokeColor = FLinearColor( 0.75f, 0.75f, 0.75f, 1.00f );
    static const FLinearColor frameColor  = FLinearColor( 0.75f, 0.75f, 0.75f, 0.25f );
    static const FLinearColor sourceColor = FLinearColor( 0.50f, 0.50f, 0.50f, 0.50f );
    static const FLinearColor interpColor = FLinearColor( 0.25f
                                                        , 0.25f
                                                        , 0.25f
                                                        , 0.25f );
    static const FLinearColor targetColor = FLinearColor( 1.0f, 0.50f, 0.50f, 0.50f );
    TArray< FVector2D > framePointBuffer;

    for( int i = 0; ( i < (int) mCellBoxBuffer.Num() ); i++ )
    {
        FLinearColor color = FLinearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        const CellBox& cellBox = mCellBoxBuffer[i];
        double midY = mBoxSize.Y * 0.5f;
        double midX = cellBox.w * 0.5f;
        uint32 visibilityGap = 0;

        if ( cellBox.type & CellBox::TYPE_SOURCE    )
            color = sourceColor;

        if ( cellBox.type & CellBox::TYPE_INBETWEEN )
            color = interpColor;

        if ( cellBox.type & CellBox::TYPE_TARGET    )
            color = sourceColor;

        FSlateDrawElement::MakeBox( OutDrawElements
                                  , LayerId
                                  , AllottedGeometry.ToPaintGeometry( FVector2D( cellBox.x
                                                                               , cellBox.y )
                                                                    , FVector2D( cellBox.w
                                                                               , cellBox.h ) )
                                  , &defaultBrush
                                  , ESlateDrawEffect::None
                                  , color );

        if( cellBox.type & CellBox::TYPE_INBETWEEN )
        {
            TArray< FVector2D > line;

            line.Reserve( 2 );
            line.Push( FVector2D( 0.0f     , midY ) );
            line.Push( FVector2D( cellBox.w, midY ) );

            FSlateDrawElement::MakeLines( OutDrawElements
                                        , LayerId
                                        , AllottedGeometry.ToPaintGeometry( FVector2D( cellBox.x
                                                                                     , cellBox.y )
                                                                          , FVector2D( cellBox.w
                                                                                     , cellBox.h ) )
                                        , line
                                        , ESlateDrawEffect::None
                                        , strokeColor
                                        , true
                                        , 2.0f );
        }

        if( cellBox.type & CellBox::TYPE_VISIBLE )
        {
            FSlateDrawElement::MakeBox( OutDrawElements
                                        , LayerId
                                        , AllottedGeometry.ToPaintGeometry( FVector2D( cellBox.x + midX - 8
                                                                                     , cellBox.y + midY - 9 )
                                                                          , FVector2D( 16
                                                                                     , 16 ) )
                                        , visibilityBrush );

            visibilityGap = 8;
        }

        if( cellBox.type & CellBox::TYPE_SOURCE )
        {
            TArray< FVector2D > line;
            TArray< FVector2D > arrow;

            line.Reserve( 2 );
            arrow.Reserve( 3 );

            // draw forward arrow
            if( mInbetweenerTag->GetInterpolationDirection() == eInbetweenerInterpolationDirection::Forward )
            {
                line.Push( FVector2D( midX + visibilityGap + 6 , midY ) );
                line.Push( FVector2D( cellBox.w, midY ) );

                if( cellBox.type & CellBox::TYPE_TARGET )
                {
                    arrow.Push( FVector2D( midX + visibilityGap + 2, midY - 5 ) );
                    arrow.Push( FVector2D( midX + visibilityGap + 6, midY     ) );
                    arrow.Push( FVector2D( midX + visibilityGap + 2, midY + 5 ) );
                }
            }

            if( mInbetweenerTag->GetInterpolationDirection() == eInbetweenerInterpolationDirection::Backward )
            {
                line.Push( FVector2D( midX - visibilityGap - 6 , midY ) );
                line.Push( FVector2D( 0.0f     , midY ) );

                if( cellBox.type & CellBox::TYPE_TARGET )
                {
                    arrow.Push( FVector2D( midX - visibilityGap - 2, midY - 5 ) );
                    arrow.Push( FVector2D( midX - visibilityGap - 6, midY     ) );
                    arrow.Push( FVector2D( midX - visibilityGap - 2, midY + 5 ) );
                }
            }

            if( arrow.Num() )
            {
                FSlateDrawElement::MakeLines( OutDrawElements
                                            , LayerId
                                            , AllottedGeometry.ToPaintGeometry( FVector2D( cellBox.x
                                                                                         , cellBox.y )
                                                                              , FVector2D( cellBox.w
                                                                                         , cellBox.h ) )
                                            , arrow
                                            , ESlateDrawEffect::None
                                            , strokeColor
                                            , true
                                            , 2.0f );
            }

            FSlateDrawElement::MakeLines( OutDrawElements
                                        , LayerId
                                        , AllottedGeometry.ToPaintGeometry( FVector2D( cellBox.x
                                                                                     , cellBox.y )
                                                                          , FVector2D( cellBox.w
                                                                                     , cellBox.h ) )
                                        , line
                                        , ESlateDrawEffect::None
                                        , strokeColor
                                        , true
                                        , 2.0f );
        }

        if( cellBox.type & CellBox::TYPE_TARGET )
        {
            TArray< FVector2D > line;
            TArray< FVector2D > arrow;

            line.Reserve( 2 );
            arrow.Reserve( 3 );

            // draw forward arrow
            if( mInbetweenerTag->GetInterpolationDirection() == eInbetweenerInterpolationDirection::Forward )
            {
                line.Push( FVector2D( 0.0f    , midY ) );
                line.Push( FVector2D( midX - visibilityGap - 2, midY ) );

                arrow.Push( FVector2D( midX - visibilityGap - 6, midY - 5 ) );
                arrow.Push( FVector2D( midX - visibilityGap - 2, midY     ) );
                arrow.Push( FVector2D( midX - visibilityGap - 6, midY + 5 ) );
            }

            // draw backward arrow
            if( mInbetweenerTag->GetInterpolationDirection() == eInbetweenerInterpolationDirection::Backward )
            {
                line.Push( FVector2D( cellBox.w, midY ) );
                line.Push( FVector2D( midX + visibilityGap + 2 , midY ) );

                arrow.Push( FVector2D( midX + visibilityGap + 6, midY - 5 ) );
                arrow.Push( FVector2D( midX + visibilityGap + 2, midY     ) );
                arrow.Push( FVector2D( midX + visibilityGap + 6, midY + 5 ) );
            }

            FSlateDrawElement::MakeLines( OutDrawElements
                                        , LayerId
                                        , AllottedGeometry.ToPaintGeometry( FVector2D( cellBox.x
                                                                                     , cellBox.y )
                                                                          , FVector2D( cellBox.w
                                                                                     , cellBox.h ) )
                                        , line
                                        , ESlateDrawEffect::None
                                        , strokeColor
                                        , true
                                        , 2.0f );

            FSlateDrawElement::MakeLines( OutDrawElements
                                        , LayerId
                                        , AllottedGeometry.ToPaintGeometry( FVector2D( cellBox.x
                                                                                     , cellBox.y )
                                                                          , FVector2D( cellBox.w
                                                                                     , cellBox.h ) )
                                        , arrow
                                        , ESlateDrawEffect::None
                                        , strokeColor
                                        , true
                                        , 2.0f );
        }
    }

    if( mCandidateTargetCellBox.type & CellBox::TYPE_TARGET )
    {
        // draw candidate target (when resizing breakdown)
        FSlateDrawElement::MakeBox( OutDrawElements
                                  , LayerId
                                  , AllottedGeometry.ToPaintGeometry( FVector2D( mCandidateTargetCellBox.x
                                                                               , mCandidateTargetCellBox.y )
                                                                    , FVector2D( mCandidateTargetCellBox.w
                                                                               , mCandidateTargetCellBox.h ) )
                                  , &defaultBrush
                                  , ESlateDrawEffect::None
                                  , targetColor );
    }

    framePointBuffer.Reserve( 4 );
    framePointBuffer.Push( FVector2D( 0.0f      , 0.0f       ) );
    framePointBuffer.Push( FVector2D( mBoxSize.X, 0.0f       ) );
    framePointBuffer.Push( FVector2D( mBoxSize.X, mBoxSize.Y ) );
    framePointBuffer.Push( FVector2D( 0.0f      , mBoxSize.Y ) );

    FSlateDrawElement::MakeLines( OutDrawElements
                                , LayerId
                                , AllottedGeometry.ToPaintGeometry( FVector2D( mBoxPos.X
                                                                             , mBoxPos.Y )
                                                                  , FVector2D( mBoxSize.X
                                                                             , mBoxSize.Y ) )
                                , framePointBuffer
                                , ESlateDrawEffect::None
                                , frameColor
                                , true
                                , 2.0f );

    return LayerId;
}

#undef LOCTEXT_NAMESPACE
