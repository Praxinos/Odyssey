// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewRow.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "OdysseyStyle.h"
#include "OdysseyVector.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "OdysseyPainterEditor.h"
#include "Undo/OdysseyVectorUndoTransferObjects.h"
#include "Undo/OdysseyVectorUndoObjectParam.h"
#include "OdysseyPainterEditorSource.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Dialogs/Dialogs.h"

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

ECheckBoxState
SOdysseyPainterEditorVectorSceneTreeViewRow::GetHierarchicalVisibility() const
{
    bool visibility = mItem->GetVectorObject()->IsVisible( true );

    return ( visibility ) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

bool
SOdysseyPainterEditorVectorSceneTreeViewRow::IsVisibilityEnabled() const
{
    return mItem->GetVectorObject()->GetParent()->IsVisible( true );
}

void
SOdysseyPainterEditorVectorSceneTreeViewRow::OnCheckBoxStateChanged( ECheckBoxState iState )
{
    const TSharedPtr< SOdysseyPainterEditorVectorSceneTreeView > treeView = StaticCastSharedPtr<SOdysseyPainterEditorVectorSceneTreeView>(OwnerTablePtr.Pin());

    // Unregister this widget's updates when the vector scene is updated. We don't want this widget to be
    // rebuilt while it's processing stuff
    treeView->UnbindLayerDelegates();

    GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.object-visibility", "Set Object Visibility"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectParam( mItem->GetVectorObject()->GetLayer()
                                                                                                      , mItem->GetVectorObject() ) );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = treeView->GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    switch( iState )
    {
        case ECheckBoxState::Checked :
            mItem->GetVectorObject()->SetVisible( true );
        break;

        case ECheckBoxState::Unchecked :
            mItem->GetVectorObject()->SetVisible( false );
        break;

        default :
        break;
    }

    mItem->GetVectorObject()->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    mItem->GetVectorObject()->GetLayer()->RequestRedraw( mItem->GetVectorObject()->GetCell(), 0 );

    treeView->BindLayerDelegates();
}

void
SOdysseyPainterEditorVectorSceneTreeViewRow::Construct( const typename STableRow<TSharedPtr<FVectorSceneTreeViewItem>>::FArguments& InArgs
                                                      , const TSharedRef< STableViewBase >& InOwnerTableView
                                                      , const TSharedPtr<FVectorSceneTreeViewItem> iItem )
{
    mItem = iItem;

    SMultiColumnTableRow <TSharedPtr<FVectorSceneTreeViewItem>>::Construct( InArgs, InOwnerTableView );
}

TSharedRef<SWidget>
SOdysseyPainterEditorVectorSceneTreeViewRow::GenerateWidgetForColumn ( const FName& InColumnName )
{
    if( InColumnName == "Visible" )
    {
        const FCheckBoxStyle* isVisibleToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("VectorSceneTreeView.IsVisibleToggle");

        return SNew(SBorder)
            .Padding(4, 0)
               .BorderBackgroundColor( FSlateColor( FLinearColor( 0, 0, 0, 0 ) ) )
               [
                   SNew( SCheckBox )
                  .IsEnabled( mItem.Get()->IsSensitive() )
                  .Style( isVisibleToggleStyle )
                  .OnCheckStateChanged( this, &SOdysseyPainterEditorVectorSceneTreeViewRow::OnCheckBoxStateChanged)
                  .IsChecked( this, &SOdysseyPainterEditorVectorSceneTreeViewRow::GetHierarchicalVisibility)
                  .IsEnabled( this, &SOdysseyPainterEditorVectorSceneTreeViewRow::IsVisibilityEnabled )
               ];
    }

    if( InColumnName == "HUD Color" )
    {
        return SNew(SBorder)
               .Padding(0, 1)
               .BorderBackgroundColor( FSlateColor( FLinearColor( 0, 0, 0, 0 ) ) )
               [
                   SNew( SColorBlock )
                  .IsEnabled( mItem->GetVectorObject()->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
                  .OnMouseButtonDown( FPointerEventHandler::CreateSP( this, &SOdysseyPainterEditorVectorSceneTreeViewRow::PickColor ) )
                  .Color_Lambda( [this]
                                 {
                                     FLinearColor LinearHUDColor = FLinearColor( mItem->GetVectorObject()->GetHUDColor() );

                                     if( mItem->GetVectorObject()->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) == false )
                                     {
                                         LinearHUDColor.A *= 0.5f;
                                     }

                                     return LinearHUDColor;
                                 } )
               ];
    }

    if( InColumnName == "Name" )
    {
        FOdysseyVectorObject* vectorObject = mItem->GetVectorObject();
        TSharedPtr<SHorizontalBox> tagBox;
        const FSlateBrush* objectIcon = nullptr;
        const FSlateBrush* inbetweenerTagIcon = nullptr;
        uint32 cellIndex = vectorObject->GetCell()->GetIndex();

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

        mTextBlockWidget = SNew(SInlineEditableTextBlock)
                           // display cell name only for insensitive objects, i.e objects from another cell
                           .Text( FText::FromString( mItem->IsSensitive() ? vectorObject->GetName()
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

        return SNew(SHorizontalBox)
                    .IsEnabled( mItem.Get()->IsSensitive() )
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(6.f, 0.f, 0.f, 0.f)
                    [
                        SNew( SExpanderArrow, SharedThis(this) ).IndentAmount(12)
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew( SImage )
                        .Image( objectIcon )
                    ]
                    + SHorizontalBox::Slot()
                    .Padding( 2, 0 )
                    .AutoWidth()
                    [
                        mTextBlockWidget.ToSharedRef()
                    ]
                    + SHorizontalBox::Slot()
                    .Padding( 10, 0 )
                    .AutoWidth()
                    [
                        tagBox.ToSharedRef()
                    ];
    }

    return SNullWidget::NullWidget;
}

FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::PickColor( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
    if ( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
    {
        const TSharedPtr< SOdysseyPainterEditorVectorSceneTreeView > treeView = StaticCastSharedPtr<SOdysseyPainterEditorVectorSceneTreeView>(OwnerTablePtr.Pin());

        FColorPickerArgs args;

        args.ParentWidget = treeView;
        args.InitialColor = FLinearColor( mItem->GetVectorObject()->GetHUDColor() );
        args.bIsModal = true;
        args.OnColorCommitted = FOnLinearColorValueChanged::CreateSP( this, &SOdysseyPainterEditorVectorSceneTreeViewRow::OnColorCommitted );

        OpenColorPicker( args );

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

void
SOdysseyPainterEditorVectorSceneTreeViewRow::OnColorCommitted( FLinearColor iColor )
{
    if( mItem.Get()->GetVectorObject()->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
    {
        FOdysseyVectorGroup* group = static_cast<FOdysseyVectorGroup*>( mItem.Get()->GetVectorObject() );

        group->SetHUDColor( iColor.ToFColor( true ) );
    }
}

FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnMouseButtonUp( const FGeometry & MyGeometry
                                                            , const FPointerEvent & MouseEvent )
{
    FOdysseyVectorGroupPaint* scene = mItem.Get()->GetVectorObject()->GetScene();
    FReply reply = FReply::Handled();

    reply = STableRow::OnMouseButtonUp( MyGeometry, MouseEvent );

    scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // request redraw
    scene->GetLayer()->RequestRedraw( scene->GetCell(), 0 );

    return reply;
}


FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnMouseButtonDown( const FGeometry & MyGeometry
                                                              , const FPointerEvent & MouseEvent )
{
    const TSharedPtr< SOdysseyPainterEditorVectorSceneTreeView > treeView = StaticCastSharedPtr<SOdysseyPainterEditorVectorSceneTreeView>(OwnerTablePtr.Pin());
    FOdysseyVectorObject* vectorObject = mItem.Get()->GetVectorObject();
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
                                | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD );

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
    const TSharedPtr< SOdysseyPainterEditorVectorSceneTreeView > treeView = StaticCastSharedPtr<SOdysseyPainterEditorVectorSceneTreeView>(OwnerTablePtr.Pin());

    // Unregister this widget's updates when the vector scene is updated. We don't want this widget to be
    // rebuilt while it's processing stuff
    //treeView->UnbindLayerDelegates();

    FOdysseyVectorObject* itemObject = mItem.Get()->GetVectorObject();
    FOdysseyVectorGroupPaint* itemScene = itemObject->GetScene();

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-object.transaction.property-changed","Property Changed"));
    if( GUndo )
    {
        FOdysseyVectorUndo *undo = new FOdysseyVectorUndoObjectParam( itemScene->GetLayer()
                                                                    , itemObject
                                                                    , FName( "Identity" ) );
        // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
        // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
        // of a call to GEditor::PostEditChangeProperty()
        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = treeView->GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    mItem.Get()->GetVectorObject()->SetName( InText.ToString() );

    mTextBlockWidget.Get()->SetText( FText::FromString( mItem.Get()->GetVectorObject()->GetName() ) );

    itemScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    //itemScene->GetLayer()->Notify( FOdysseyPainterEditor::UI_UPDATE_TIMELINE );

    //treeView->BindLayerDelegates();
}

FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnDrop( const FGeometry& iGeometry
                                                   , const FDragDropEvent& iDragDropEvent )
{
    const TSharedPtr< SOdysseyPainterEditorVectorSceneTreeView > treeView = StaticCastSharedPtr<SOdysseyPainterEditorVectorSceneTreeView>(OwnerTablePtr.Pin());
    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    //FVector2D position = iGeometry.GetAbsolutePosition();
    FOdysseyVectorObject* itemObject = mItem.Get()->GetVectorObject();
    FOdysseyVectorGroupPaint* itemScene = itemObject->GetScene();
    std::list<FOdysseyVectorObject*> focusedObjectList;
    FOdysseyVectorObject* insertObject = itemObject;

    // Unregister this widget's updates when the vector scene is updated. We don't want this widget to be
    // rebuilt while it's processing stuff
    //treeView->UnbindLayerDelegates();

    itemScene->GetCell()->GetFocusedAncestorList( focusedObjectList );

    GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.drag-drop-object", "Drop Objects"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoTransferObjects( itemScene
                                                                                                          , focusedObjectList ) );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

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

                // note: Layer and Cell are system objects
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

    itemScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    itemScene->GetLayer()->RequestRedraw( itemScene->GetCell(), 0 );

    //treeView->BindLayerDelegates();

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
