// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewRow.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "OdysseyStyleSet.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoTransferObjects.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FVectorSceneTreeViewItem::~FVectorSceneTreeViewItem()
{
}

FVectorSceneTreeViewItem::FVectorSceneTreeViewItem( FOdysseyVectorObject* iVectorObject )
{
    mVectorObject = iVectorObject;
}

FOdysseyVectorObject*
FVectorSceneTreeViewItem::GetVectorObject()
{
    return mVectorObject;
}

SOdysseyPainterEditorVectorSceneTreeViewRow::~SOdysseyPainterEditorVectorSceneTreeViewRow()
{
}

SOdysseyPainterEditorVectorSceneTreeViewRow::SOdysseyPainterEditorVectorSceneTreeViewRow()
    : mDropZone( DROPZONE_NONE )
{
}

void
SOdysseyPainterEditorVectorSceneTreeViewRow::Construct( const typename STableRow<TSharedPtr<FVectorSceneTreeViewItem>>::FArguments& InArgs
                                                      , const TSharedRef< STableViewBase >& InOwnerTableView
                                                      , const TSharedPtr<FVectorSceneTreeViewItem> iItem )
{
    STableRow<TSharedPtr<FVectorSceneTreeViewItem>>::Construct( InArgs, InOwnerTableView );
    FOdysseyVectorObject* vectorObject = iItem->GetVectorObject();
    const FSlateBrush* icon = nullptr;

    if ( vectorObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        icon = FOdysseyStyle::GetBrush( "PainterEditor.VectorSceneTreeView.Paintgroup16" );
    }
    else
    if ( vectorObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
    {
        icon = FOdysseyStyle::GetBrush( "PainterEditor.VectorSceneTreeView.Group16" );
    }
    else
    if ( vectorObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        icon = FOdysseyStyle::GetBrush( "PainterEditor.VectorSceneTreeView.Path16" );
    }
    else
    {
        icon = FOdysseyStyle::GetBrush( "PainterEditor.VectorSceneTreeView.null16" );
    }

    mItem = iItem;

    mTextBlockWidget = SNew(SInlineEditableTextBlock)
                       .Text( FText::FromString( mItem.Get()->GetVectorObject()->GetName() ) )
                       .OnVerifyTextChanged( this, &SOdysseyPainterEditorVectorSceneTreeViewRow::OnVerifyTextChanged )
                       .OnTextCommitted( this, &SOdysseyPainterEditorVectorSceneTreeViewRow::OnTextChanged );

    SetContent( SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SImage )
                    .Image( icon )
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    mTextBlockWidget.ToSharedRef()
                ] );

    //SetContent( mTextBlockWidget.ToSharedRef() );
}

bool
SOdysseyPainterEditorVectorSceneTreeViewRow::OnVerifyTextChanged( const FText& NewText
                                                                , FText& OutErrorMessage )
{
    return true;
}

void
SOdysseyPainterEditorVectorSceneTreeViewRow::Rename()
{
    mTextBlockWidget.Get()->EnterEditingMode();
}

void
SOdysseyPainterEditorVectorSceneTreeViewRow::OnTextChanged( const FText& InText
                                                          , ETextCommit::Type CommitInfo )
{
    mItem.Get()->GetVectorObject()->SetName( InText.ToString() );

    mTextBlockWidget.Get()->SetText( FText::FromString( mItem.Get()->GetVectorObject()->GetName() ) );
}

FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnDrop( const FGeometry& iGeometry
                                                   , const FDragDropEvent& iDragDropEvent )
{
    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    //FVector2D position = iGeometry.GetAbsolutePosition();
    FOdysseyVectorObject* itemObject = mItem.Get()->GetVectorObject();
    FOdysseyVectorGroupPaint* itemScene = itemObject->GetScene();
    std::list<FOdysseyVectorObject*> focusedObjectList;
    FOdysseyVectorObject* insertObject = itemObject;

    itemScene->GetEngine()->GetFocusedAncestorList( focusedObjectList );

    GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.drag-drop-object", "Drop Objects"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoTransferObjects( itemScene, focusedObjectList ) );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        switch( mDropZone )
        {
            //case DROPZONE_ABOVE:
            // reverse order in order to get the most forward objet on top of the hierarchy 
            case DROPZONE_BELOW:
            {
                FOdysseyVectorObject* parentObject = itemObject->GetParent();

                // don't drop onto the same object or else expect some infinite loop
                if( parentObject != focusedObject )
                {
                    parentObject->TransferChild( focusedObject, parentObject->GetPreviousChild( insertObject ) );

                    insertObject = focusedObject;
                }
            }
            break;

            case DROPZONE_ONTO:
                // don't drop onto the same object or else expect some infinite loop
                if( itemObject != focusedObject )
                {
                    itemObject->TransferChild( focusedObject, nullptr );
                }
            break;

            // case DROPZONE_BELOW:
            // reverse order in order to get the most forward objet on top of the hierarchy 
            case DROPZONE_ABOVE:
            {
                FOdysseyVectorObject* parentObject = itemObject->GetParent();

                // don't drop onto the same object or else expect some infinite loop
                if( parentObject != focusedObject )
                {
                    parentObject->TransferChild( focusedObject, insertObject );

                    insertObject = focusedObject;
                }
            }
            break;

            default :
            break;
        }
    }

    mDropZone = DROPZONE_NONE;

    itemScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    itemScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                                  | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                                  | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                                  | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return FReply::Handled();
}

int32
SOdysseyPainterEditorVectorSceneTreeViewRow::OnPaint( const FPaintArgs& Args
                                                    , const FGeometry& AllottedGeometry
                                                    , const FSlateRect& MyCullingRect
                                                    , FSlateWindowElementList& OutDrawElements
                                                    , int32 LayerId
                                                    , const FWidgetStyle& InWidgetStyle
                                                    , bool bParentEnabled ) const
{
    const FTableRowStyle& style = FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows");
    const FSlateBrush* DropIndicatorBrush = nullptr;

    int32 rowLayerId = STableRow<TSharedPtr<FVectorSceneTreeViewItem>>::OnPaint( Args
                                                                               , AllottedGeometry
                                                                               , MyCullingRect
                                                                               , OutDrawElements
                                                                               , LayerId
                                                                               , InWidgetStyle
                                                                               , bParentEnabled );
    if( mDropZone )
    {
        switch( mDropZone )
        {
            case DROPZONE_ABOVE :
                DropIndicatorBrush = &style.DropIndicator_Above;
            break;

            case DROPZONE_ONTO :
                DropIndicatorBrush = &style.DropIndicator_Onto;
            break;

            case DROPZONE_BELOW :
                DropIndicatorBrush = &style.DropIndicator_Below;
            break;

            default :
            break;
        }

        FSlateDrawElement::MakeBox
        (
            OutDrawElements
          , rowLayerId++
          , AllottedGeometry.ToPaintGeometry()
          , DropIndicatorBrush //&myBrush
          , ESlateDrawEffect::None
          , DropIndicatorBrush->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
        );
    }

    return rowLayerId;
}

void
SOdysseyPainterEditorVectorSceneTreeViewRow::OnDragEnter( const FGeometry& MyGeometry
                                                        , const FDragDropEvent& DragDropEvent )
{
    //const TSharedPtr<FVectorSceneTreeViewItem>* item = GetItemForThis( OwnerTablePtr.Pin().ToSharedRef() );

    mDropZone = DROPZONE_ONTO;
}

void
SOdysseyPainterEditorVectorSceneTreeViewRow::OnDragLeave( const FDragDropEvent& DragDropEvent )
{
    //const TSharedPtr<FVectorSceneTreeViewItem>* item = GetItemForThis( OwnerTablePtr.Pin().ToSharedRef() );

    mDropZone = DROPZONE_NONE;
}

FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnDragOver( const FGeometry& iGeometry
                                                       , const FDragDropEvent& iDragDropEvent )
{
    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    //FVector2D position = iGeometry.GetAbsolutePosition();
    const FVector2D localPointerPos = iGeometry.AbsoluteToLocal( iDragDropEvent.GetScreenSpacePosition() );
    const FVector2D& widgetSize = iGeometry.GetLocalSize();

    if( localPointerPos.Y < 5 )
    {
        mDropZone = DROPZONE_ABOVE;
    }
    else
    if( localPointerPos.Y > widgetSize.Y - 5 )
    {
        mDropZone = DROPZONE_BELOW;
    }
    else
    {
        mDropZone = DROPZONE_ONTO;
    }

    return FReply::Handled();
}

FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnDragDetected ( const FGeometry& iGeometry,
                                                              const FPointerEvent& iMouseEvent )
{
    TSharedRef<FDragDropOperation> Operation = MakeShared<FDragDropOperation>();

    return FReply::Handled().BeginDragDrop(Operation);
}

#undef LOCTEXT_NAMESPACE
