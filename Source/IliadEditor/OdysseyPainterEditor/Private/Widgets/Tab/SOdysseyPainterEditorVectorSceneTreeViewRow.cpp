// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewRow.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "OdysseyStyleSet.h"
#include "OdysseyVector.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorCell.h"
#include "OdysseyPainterEditor.h"
#include "Undo/OdysseyVectorUndoTransferObjects.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FVectorSceneTreeViewItem::~FVectorSceneTreeViewItem()
{
}

FVectorSceneTreeViewItem::FVectorSceneTreeViewItem( FOdysseyVectorObject* iVectorObject, bool iSensitive )
    : mVectorObject( iVectorObject )
    , bSensitive( iSensitive )
{
}

FOdysseyVectorObject*
FVectorSceneTreeViewItem::GetVectorObject()
{
    return mVectorObject;
}

bool
FVectorSceneTreeViewItem::IsSensitive()
{
    return bSensitive;
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
    TSharedPtr<SHorizontalBox> tagBox;
    const FSlateBrush* objectIcon = nullptr;
    const FSlateBrush* inbetweenerTagIcon = nullptr;
    uint32 cellIndex = vectorObject->GetEngine()->GetCell()->GetIndex();

    //inbetweenerTagIcon = FOdysseyStyle::GetBrush( "PainterEditor.VectorSceneTreeView.InbetweenerTag16" );
    inbetweenerTagIcon = FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Matching16" );

    if ( vectorObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        objectIcon = FOdysseyStyle::GetBrush( "PainterEditor.VectorSceneTreeView.Paintgroup" );
    }
    else
    if ( vectorObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
    {
        objectIcon = FOdysseyStyle::GetBrush( "PainterEditor.VectorSceneTreeView.Group" );
    }
    else
    if ( vectorObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        objectIcon = FOdysseyStyle::GetBrush( "PainterEditor.VectorSceneTreeView.Path" );
    }
    else
    {
        objectIcon = FOdysseyStyle::GetBrush( "PainterEditor.VectorSceneTreeView.null16" );
    }

    mItem = iItem;

    mTextBlockWidget = SNew(SInlineEditableTextBlock)
                       // display cell name only for insensitive objects, i.e objects from another cell
                       .Text( FText::FromString( iItem->IsSensitive() ? vectorObject->GetName()
                                                                      : FString::Printf( TEXT("Cell %d / "), cellIndex )
                                                                      + vectorObject->GetName() ) )
                       .OnVerifyTextChanged( this, &SOdysseyPainterEditorVectorSceneTreeViewRow::OnVerifyTextChanged )
                       .OnTextCommitted( this, &SOdysseyPainterEditorVectorSceneTreeViewRow::OnTextChanged );

    tagBox = SNew(SHorizontalBox);

    for( FOdysseyVectorTag* tag : vectorObject->GetTagList() )
    {
        if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
        {
            //Cells widgets
            tagBox->AddSlot()
            .AutoWidth()
            [
                SNew( SImage )
                .Image( inbetweenerTagIcon )
            ];
        }
    }

    SetContent( SNew(SHorizontalBox)
                .IsEnabled( mItem.Get()->IsSensitive() )
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SImage )
                    .Image( objectIcon )
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    mTextBlockWidget.ToSharedRef()
                ]
                + SHorizontalBox::Slot()
                .Padding( 10, 0 )
                .AutoWidth()
                [
                    tagBox.ToSharedRef()
                ] );
}

FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnMouseButtonUp( const FGeometry & MyGeometry
                                                            , const FPointerEvent & MouseEvent )
{
    FReply reply = FReply::Handled();

    reply = STableRow::OnMouseButtonUp( MyGeometry, MouseEvent );

    // request redraw
    mItem.Get()->GetVectorObject()->GetEngine()->Invalidate( 0 );

    FOdysseyVectorEngine::Notify( nullptr,
                                  FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                                //| FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                                | FOdysseyPainterEditor::UI_UPDATE_HUD );

    return reply;
}


FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnMouseButtonDown( const FGeometry & MyGeometry
                                                              , const FPointerEvent & MouseEvent )
{
    const TSharedPtr< SOdysseyPainterEditorVectorSceneTreeView > treeView = StaticCastSharedPtr<SOdysseyPainterEditorVectorSceneTreeView>(OwnerTablePtr.Pin());
    FOdysseyVectorObject* vectorObject = mItem.Get()->GetVectorObject();
    FOdysseyVectorEngine* vectorEngine = vectorObject->GetEngine();
    FReply reply = FReply::Handled();
/*
    if( mItem.Get()->IsSensitive() == false )
    {
        return FReply::Unhandled();
    }
*/
/*
    if( vectorObject->IsSelected() == false )
    {
        vectorEngine->SelectObject( vectorObject );
    }
*/
    reply = STableRow::OnMouseButtonDown( MyGeometry, MouseEvent );


    return reply;
}

/*
FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnMouseButtonDown( const FGeometry & MyGeometry
                                                              , const FPointerEvent & MouseEvent )
{
    const TSharedPtr< SOdysseyPainterEditorVectorSceneTreeView > treeView = StaticCastSharedPtr<SOdysseyPainterEditorVectorSceneTreeView>(OwnerTablePtr.Pin());
    FOdysseyVectorObject* vectorObject = mItem.Get()->GetVectorObject();
    FOdysseyVectorGroupPaint* vectorScene = vectorObject->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorObject->GetEngine();
    FReply reply = FReply::Handled();

    if( mItem.Get()->IsSensitive() == false )
    {
        return FReply::Unhandled();
    }

    if ( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
    {
          reply
          .DetectDrag(SharedThis(this), EKeys::LeftMouseButton)
          .SetUserFocus(treeView->AsWidget(), EFocusCause::Mouse);
    }

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

            for( const TSharedPtr<FVectorSceneTreeViewItem>& item : treeView.Get()->GetItems() )
            {
                FOdysseyVectorObject* itemObject = item.Get()->GetVectorObject();

                if( ( itemObject == vectorObject ) || ( itemObject == lastSelectedObject ) )
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
            vectorEngine->ClearObjectSelection();
        }
    }

    if( vectorObject->IsSelected() == false )
    {
        vectorEngine->SelectObject( vectorObject );
    }
    else
    {
        if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
        {
            vectorEngine->UnselectObject( vectorObject );
        }
    }

    // request redraw
    mItem.Get()->GetVectorObject()->GetEngine()->Invalidate( 0 );

    FOdysseyVectorEngine::Notify( nullptr,
                                  FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                                | FOdysseyPainterEditor::UI_UPDATE_HUD );

    return reply;
}
*/

ESelectionMode::Type
SOdysseyPainterEditorVectorSceneTreeViewRow::GetSelectionMode () const
{
    return mItem.Get()->IsSensitive() ? ESelectionMode::Type::Multi :  ESelectionMode::Type::None;
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
    FOdysseyVectorObject* itemObject = mItem.Get()->GetVectorObject();
    FOdysseyVectorGroupPaint* itemScene = itemObject->GetScene();

    mItem.Get()->GetVectorObject()->SetName( InText.ToString() );

    mTextBlockWidget.Get()->SetText( FText::FromString( mItem.Get()->GetVectorObject()->GetName() ) );

    FOdysseyVectorEngine::Notify( itemScene,
                                  FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                                | FOdysseyPainterEditor::UI_UPDATE_TIMELINE );
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
    uint32 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE;

    itemScene->GetEngine()->GetFocusedAncestorList( focusedObjectList );

    GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.drag-drop-object", "Drop Objects"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoTransferObjects( itemScene
                                                                                                          , focusedObjectList
                                                                                                          , notificationFlags ) );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        const TSharedPtr< SOdysseyPainterEditorVectorSceneTreeView > treeView = StaticCastSharedPtr<SOdysseyPainterEditorVectorSceneTreeView>(OwnerTablePtr.Pin());
        TSharedPtr<FOdysseyPainterEditorSource> source = treeView->GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
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

                // note: SharedEnv and Root are system objects
                if( parentObject->IsSystem() == false )
                {
                    // don't drop onto the same object or else expect some infinite loop
                    if( parentObject != focusedObject )
                    {
                        parentObject->TransferChild( focusedObject, parentObject->GetPreviousChild( insertObject ) );

                        insertObject = focusedObject;
                    }
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

                // note: SharedEnv and Root are system objects
                if( parentObject->IsSystem() == false )
                {

                    if( parentObject != focusedObject )
                    {
                        parentObject->TransferChild( focusedObject, insertObject );

                        insertObject = focusedObject;
                    }
                }
            }
            break;

            default :
            break;
        }
    }

    mDropZone = DROPZONE_NONE;

    itemScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    itemScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( itemScene, notificationFlags );

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
