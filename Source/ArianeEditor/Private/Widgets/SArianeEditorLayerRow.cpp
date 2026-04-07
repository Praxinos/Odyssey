// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "SArianeEditorLayerRow.h"
#include "SArianeEditorLayerStack.h"
#include "ArianeEditor.h"
#include "ArianeLayer.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"
// Odyssey headers
#include "OdysseyStyle.h"
// Unreal headers
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Dialogs/Dialogs.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

FArianeEditorLayerRowItem::~FArianeEditorLayerRowItem()
{
}

FArianeEditorLayerRowItem::FArianeEditorLayerRowItem( UArianeLayer* InLayer, bool bInSensitive )
    : Layer( InLayer )
    , bSensitive( bInSensitive )
{
}

UArianeLayer*
FArianeEditorLayerRowItem::GetLayer()
{
    return Layer;
}

bool
FArianeEditorLayerRowItem::IsSensitive()
{
    return bSensitive;
}

SArianeEditorLayerRow::~SArianeEditorLayerRow()
{
}

SArianeEditorLayerRow::SArianeEditorLayerRow()
    : DropZone( DROPZONE_NONE )
{
}

ECheckBoxState
SArianeEditorLayerRow::IsHierarchicallyVisible() const
{
    bool visibility = Item->GetLayer()->IsVisible();

    return ( visibility ) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

bool
SArianeEditorLayerRow::IsVisibleCheckBoxEnabled() const
{
    return Item->GetLayer()->GetParent() ? Item->GetLayer()->GetParent()->IsVisible() : true;
}

ECheckBoxState
SArianeEditorLayerRow::IsHierarchicallyLocked() const
{
    bool locked = Item->GetLayer()->IsLocked( true );

    return ( locked ) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

bool
SArianeEditorLayerRow::IsLockedCheckBoxEnabled() const
{
    return Item->GetLayer()->GetParent() ? Item->GetLayer()->GetParent()->IsLocked( true ) == false : true;
}

void
SArianeEditorLayerRow::OnVisibleStateChanged( ECheckBoxState iState )
{
    const TSharedPtr< SArianeEditorLayerStack > treeView = StaticCastSharedPtr<SArianeEditorLayerStack>(OwnerTablePtr.Pin());

    switch( iState )
    {
        case ECheckBoxState::Checked :
            Item->GetLayer()->SetVisibility( true );
        break;

        case ECheckBoxState::Unchecked :
            Item->GetLayer()->SetVisibility( false );
        break;

        default :
        break;
    }
}

void
SArianeEditorLayerRow::OnLockedStateChanged( ECheckBoxState iState )
{
    const TSharedPtr< SArianeEditorLayerStack > treeView = StaticCastSharedPtr<SArianeEditorLayerStack>(OwnerTablePtr.Pin());

    switch( iState )
    {
        case ECheckBoxState::Checked :
            Item->GetLayer()->SetLocked( true );
        break;

        case ECheckBoxState::Unchecked :
            Item->GetLayer()->SetLocked( false );
        break;

        default :
        break;
    }
}

void
SArianeEditorLayerRow::Construct( const typename STableRow<TSharedPtr<FArianeEditorLayerRowItem>>::FArguments& InArgs
                                , const TSharedRef< STableViewBase >& InOwnerTableView
                                , const TSharedPtr<FArianeEditorLayerRowItem> iItem )
{
    Item = iItem;

    SMultiColumnTableRow <TSharedPtr<FArianeEditorLayerRowItem>>::Construct( InArgs, InOwnerTableView );
}

TSharedRef<SWidget>
SArianeEditorLayerRow::GenerateWidgetForColumn ( const FName& InColumnName )
{
    UArianeLayer* Layer = Item->GetLayer();

    if( InColumnName == SArianeEditorLayerStack::LAYER_VISIBLE )
    {
        const FCheckBoxStyle* isVisibleToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("ArianeLayerStack.IsVisibleToggle");

        return SNew(SHorizontalBox)
               + SHorizontalBox::Slot()
               .AutoWidth()
               .HAlign( EHorizontalAlignment::HAlign_Center )
               .VAlign( EVerticalAlignment::VAlign_Center )
               [
                   SNew( SCheckBox )
                  .IsEnabled( Item.Get()->IsSensitive() )
                  .Style( isVisibleToggleStyle )
                  .OnCheckStateChanged( this, &SArianeEditorLayerRow::OnVisibleStateChanged)
                  .IsChecked( this, &SArianeEditorLayerRow::IsHierarchicallyVisible)
                  .IsEnabled( this, &SArianeEditorLayerRow::IsVisibleCheckBoxEnabled )
               ];
    }

    if( InColumnName == SArianeEditorLayerStack::LAYER_LOCKED )
    {
        const FCheckBoxStyle* isLockedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("ArianeLayerStack.IsLockedToggle");

        return SNew(SHorizontalBox)
               + SHorizontalBox::Slot()
               .AutoWidth()
               .HAlign( EHorizontalAlignment::HAlign_Center )
               .VAlign( EVerticalAlignment::VAlign_Center )
               [
                   SNew( SCheckBox )
                  .IsEnabled( Item.Get()->IsSensitive() )
                  .Style( isLockedToggleStyle )
                  .OnCheckStateChanged( this, &SArianeEditorLayerRow::OnLockedStateChanged)
                  .IsChecked( this, &SArianeEditorLayerRow::IsHierarchicallyLocked)
                  .IsEnabled( this, &SArianeEditorLayerRow::IsLockedCheckBoxEnabled )
               ];
    }

    if( InColumnName == SArianeEditorLayerStack::LAYER_NAME )
    {
        const FSlateBrush* objectIcon = nullptr;

        if ( Cast<UArianeLayerFolder>(Layer) )
        {
            objectIcon = FOdysseyStyle::GetBrush( "PainterEditor.Layers16" );
        }
        else
        if ( Cast<UArianeLayerDrawing>(Layer) )
        {
            objectIcon = FOdysseyStyle::GetBrush( "PainterEditor.Layers16" );
        }

        TextBlockWidget = SNew(SInlineEditableTextBlock)
                           // display cell name only for insensitive objects, i.e objects from another cell
                           .Text( FText::FromString( Layer->GetName() ) )
                           .ToolTipText_Lambda( [ Layer ]
                                                {
                                                    return FText::FromString( *Layer->GetName() );
                                                } )
                           .OnVerifyTextChanged( this, &SArianeEditorLayerRow::OnVerifyTextChanged )
                           .OnTextCommitted( this, &SArianeEditorLayerRow::OnTextChanged )
                           .IsSelected(this, &SArianeEditorLayerRow::IsLayerSelected );

        TextBlockWidget.Get()->SetOverflowPolicy( TOptional<ETextOverflowPolicy>(ETextOverflowPolicy::Ellipsis) );

        return SNew(SHorizontalBox)
               .IsEnabled( Item.Get()->IsSensitive() )
               +SHorizontalBox::Slot()
               .AutoWidth()
               .Padding(6.f, 0.f, 0.f, 0.f)
               [
                   SNew( SExpanderArrow, SharedThis(this) ).IndentAmount(12)
               ]
               + SHorizontalBox::Slot()
               .AutoWidth()
               .VAlign( EVerticalAlignment::VAlign_Center )
               .HAlign( EHorizontalAlignment::HAlign_Center )
               [
                   SNew( SImage )
                   .Image( objectIcon )
               ]
               + SHorizontalBox::Slot()
               .Padding( 2, 0 )
               .AutoWidth()
               [
                   TextBlockWidget.ToSharedRef()
               ];
    }

    return SNullWidget::NullWidget;
}

FReply
SArianeEditorLayerRow::OnMouseButtonUp( const FGeometry & MyGeometry
                                      , const FPointerEvent & MouseEvent )
{
    FReply reply = FReply::Handled();

    reply = SMultiColumnTableRow::OnMouseButtonUp( MyGeometry, MouseEvent );

    return reply;
}


FReply
SArianeEditorLayerRow::OnMouseButtonDown( const FGeometry & MyGeometry
                                        , const FPointerEvent & MouseEvent )
{
    const TSharedPtr< SArianeEditorLayerStack > treeView = StaticCastSharedPtr<SArianeEditorLayerStack>(OwnerTablePtr.Pin());
    UArianeLayer* Layer = Item.Get()->GetLayer();

    return SMultiColumnTableRow::OnMouseButtonDown( MyGeometry, MouseEvent );
}

ESelectionMode::Type
SArianeEditorLayerRow::GetSelectionMode () const
{
    return Item.Get()->IsSensitive() ? ESelectionMode::Type::Multi :  ESelectionMode::Type::None;
}

bool
SArianeEditorLayerRow::OnVerifyTextChanged( const FText& NewText
                                          , FText& OutErrorMessage )
{
    return true;
}

void
SArianeEditorLayerRow::Rename()
{
    TextBlockWidget.Get()->EnterEditingMode();
}

void
SArianeEditorLayerRow::OnTextChanged( const FText& InText
                                    , ETextCommit::Type CommitInfo )
{
    const TSharedPtr< SArianeEditorLayerStack > treeView = StaticCastSharedPtr<SArianeEditorLayerStack>(OwnerTablePtr.Pin());

    UArianeLayer* ItemLayer = Item.Get()->GetLayer();

    ItemLayer->Rename( *InText.ToString() );

    TextBlockWidget.Get()->SetText( ItemLayer->GetName() );
}

FReply
SArianeEditorLayerRow::OnDrop( const FGeometry& iGeometry
                             , const FDragDropEvent& iDragDropEvent )
{
    const TSharedPtr< SArianeEditorLayerStack > TreeView = StaticCastSharedPtr<SArianeEditorLayerStack>(OwnerTablePtr.Pin());
    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    UArianeLayer* ItemLayer = Item.Get()->GetLayer();

    const TArray<UArianeLayer*>& SelectedLayers = TreeView->GetEditor()->GetCurrentPainting3DComponent()->GetLayerStack()->GetSelectedLayers();

    for( UArianeLayer* SelectedLayer : SelectedLayers )
    {
        switch( DropZone )
        {
            //case DROPZONE_ABOVE:
            // reverse order in order to get the most forward objet on top of the hierarchy
            case DROPZONE_BELOW:
            {
                UArianeLayerFolder* ParentFolder = ItemLayer->GetParent();

                // don't drop onto the same object or else expect some infinite loop
                if( ParentFolder != SelectedLayer )
                {
                    //ParentFolder->TransferChild( SelectedLayer, ParentFolder->GetPreviousChild( insertObject ) );

                    //insertObject = SelectedLayer;
                }
            }
            break;

            case DROPZONE_ONTO:
                if( ItemLayer->GetClass() == UArianeLayerFolder::StaticClass() )
                {
                    UArianeLayerFolder* ItemLayerFolder = Cast<UArianeLayerFolder>(ItemLayer);

                    // don't drop onto the same object or else expect some infinite loop
                    if( ItemLayerFolder != SelectedLayer )
                    {
                        //ItemLayerFolder->TransferChild( SelectedLayer, nullptr );
                    }
                }
            break;

            // case DROPZONE_BELOW:
            // reverse order in order to get the most forward objet on top of the hierarchy
            case DROPZONE_ABOVE:
            {
                UArianeLayerFolder* ParentFolder = ItemLayer->GetParent();

                if( ParentFolder != SelectedLayer )
                {
                    //ParentFolder->TransferChild( SelectedLayer, insertObject );

                    //insertObject = SelectedLayer;
                }
            }
            break;

            default :
            break;
        }
    }

    DropZone = DROPZONE_NONE;

    return FReply::Handled();
}

bool
SArianeEditorLayerRow::IsLayerSelected() const
{
    UArianeLayer* ItemLayer = Item.Get()->GetLayer();

    return ItemLayer->IsSelected();
}

int32
SArianeEditorLayerRow::OnPaint( const FPaintArgs& Args
                              , const FGeometry& AllottedGeometry
                              , const FSlateRect& MyCullingRect
                              , FSlateWindowElementList& OutDrawElements
                              , int32 LayerId
                              , const FWidgetStyle& InWidgetStyle
                              , bool bParentEnabled ) const
{
    const FTableRowStyle& style = FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows");
    const FSlateBrush* DropIndicatorBrush = nullptr;

    int32 rowLayerId = SMultiColumnTableRow<TSharedPtr<FArianeEditorLayerRowItem>>::OnPaint( Args
                                                                                           , AllottedGeometry
                                                                                           , MyCullingRect
                                                                                           , OutDrawElements
                                                                                           , LayerId
                                                                                           , InWidgetStyle
                                                                                           , bParentEnabled );
    if( DropZone )
    {
        switch( DropZone )
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
SArianeEditorLayerRow::OnDragEnter( const FGeometry& MyGeometry
                                  , const FDragDropEvent& DragDropEvent )
{
    DropZone = DROPZONE_ONTO;
}

void
SArianeEditorLayerRow::OnDragLeave( const FDragDropEvent& DragDropEvent )
{
    DropZone = DROPZONE_NONE;
}

FReply
SArianeEditorLayerRow::OnDragOver( const FGeometry& iGeometry
                                 , const FDragDropEvent& iDragDropEvent )
{
    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    const FVector2D localPointerPos = iGeometry.AbsoluteToLocal( iDragDropEvent.GetScreenSpacePosition() );
    const FVector2D& widgetSize = iGeometry.GetLocalSize();

    if( localPointerPos.Y < 5 )
    {
        DropZone = DROPZONE_ABOVE;
    }
    else
    if( localPointerPos.Y > widgetSize.Y - 5 )
    {
        DropZone = DROPZONE_BELOW;
    }
    else
    {
        DropZone = DROPZONE_ONTO;
    }

    return FReply::Handled();
}

FReply
SArianeEditorLayerRow::OnDragDetected ( const FGeometry& iGeometry
                                      , const FPointerEvent& iMouseEvent )
{
    TSharedRef<FDragDropOperation> Operation = MakeShared<FDragDropOperation>();

    return FReply::Handled().BeginDragDrop( Operation );
}

#undef LOCTEXT_NAMESPACE
