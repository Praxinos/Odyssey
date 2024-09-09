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
    // compute geometry
    FOdysseyAnimationEditorExtension* animationEditorExtension = treeView.Get()->GetAnimationEditorExtension();
    const FVector2D cursorPos = MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() );
    FOdysseyVectorGroupPaint* scene = mInbetweenerTag->GetOwner()->GetScene();
    FOdysseyVectorSharedEnv* sharedEnv = mInbetweenerTag->GetOwner()->GetEngine()->GetSharedEnv();
    std::list<FOdysseyVectorTag*>& sharedTagList = sharedEnv->GetSharedTagList();
    uint64 retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;

    // for AddBreakdown / RemoveBreakdown functions in the context menu
    treeView.Get()->SetCursorPos( cursorPos );

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
            uint32 bi = breakdown->GetTargetDrawingIndex();

            if( ( cursorPos.X >   mCellBoxBuffer[bi].x )
             && ( cursorPos.Y >   mCellBoxBuffer[bi].y )
             && ( cursorPos.X < ( mCellBoxBuffer[bi].x + mCellBoxBuffer[bi].w ) )
             && ( cursorPos.Y < ( mCellBoxBuffer[bi].y + mCellBoxBuffer[bi].h ) ) )
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
            float frameWidth = animationEditorExtension->Timeline()->GetFrameWidth();

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
                    //mPickedBreakdown->SetTargetDrawingIndex( inbetweenCellIndex );

                    mCandidateTargetCellBox = FInbetweeningRowCellBox( EInbetweeningRowCellBoxType::Target
                                                                     , inbetweenCellIndex
                                                                     , cell->GetFrame() * frameWidth * mLayoutScaleMultiplier
                                                                     , 0.0f
                                                                     , cell->GetLength() * frameWidth * mLayoutScaleMultiplier
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
    uint64 retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                    | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                    | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                    | FOdysseyPainterEditor::UI_UPDATE_HUD;
    FReply reply = FReply::Unhandled();

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
        mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->Signal( retFlags );

        reply = FReply::Handled();
    }

    mPickedBreakdown = nullptr;
    mCandidateTargetCellBox.type = EInbetweeningRowCellBoxType::None;

    return reply;
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

    // call from base class
    STableRow<TSharedPtr<FInbetweeningListViewItem>>::CacheDesiredSize( LayoutScaleMultiplier );

    mLayoutScaleMultiplier = LayoutScaleMultiplier;

    // total size of the widget
    mBoxPos.X  = sourceFrame * frameWidth * LayoutScaleMultiplier;
    mBoxPos.Y  = 0.0f;
    mBoxSize.Y = 20.0f * LayoutScaleMultiplier;

    mCellBoxBuffer.Reset();
    mCellBoxBuffer.Reserve( mInbetweenerTag->GetDrawingCount() );

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
        IOdysseyVectorAnimationCell* targetCell = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetCellByIndex( tagCellIndex + breakdown->GetTargetDrawingIndex() );
        uint32 targetFrame = targetCell->GetFrame();

        for( uint32 i = sourceIndex + 1; i < targetIndex; i++ )
        {
            IOdysseyVectorAnimationCell* inbetweenCell = mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->GetAnimationCell()->GetCellByIndex( tagCellIndex + i );
            uint32 inbetweenFrame = inbetweenCell->GetFrame();

            mCellBoxBuffer.Emplace( EInbetweeningRowCellBoxType::Inbetween
                                  , i
                                  , inbetweenFrame * frameWidth * LayoutScaleMultiplier
                                  , 0.0f
                                  , inbetweenCell->GetLength() * frameWidth * LayoutScaleMultiplier
                                  , mBoxSize.Y );
        }

        mCellBoxBuffer.Emplace( EInbetweeningRowCellBoxType::Target
                              , targetIndex
                              , targetFrame * frameWidth * LayoutScaleMultiplier
                              , 0.0f
                              , targetCell->GetLength() * frameWidth * LayoutScaleMultiplier
                              , mBoxSize.Y );
    }

    mBoxSize.X = mCellBoxBuffer.Last().x + mCellBoxBuffer.Last().w;
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
    FLinearColor targetColor = FLinearColor( 1.0f, 0.5f, 0.0f, 0.25f );
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
		                          , AllottedGeometry.ToPaintGeometry( FVector2D( mCellBoxBuffer[i].x
                                                                               , mCellBoxBuffer[i].y )
                                                                    , FVector2D( mCellBoxBuffer[i].w
                                                                               , mCellBoxBuffer[i].h ) )
		                          , &defaultBrush
		                          , ESlateDrawEffect::None
		                          , color );
    }

    if( mCandidateTargetCellBox.type == EInbetweeningRowCellBoxType::Target )
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
