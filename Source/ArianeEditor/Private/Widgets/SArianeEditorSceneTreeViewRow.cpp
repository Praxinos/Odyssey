// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor Headers
#include "SArianeEditorSceneTreeViewRow.h"
#include "SArianeEditorSceneTreeView.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
// Ariane Headers
#include "ArianeObject.h"
#include "ArianeGroup.h"
#include "ArianeRectangle.h"
#include "ArianeEllipse.h"
#include "ArianeLine.h"
#include "ArianePolygon.h"
#include "ArianeImage.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"
// Unreal Headers
#include "Editor.h"
#include "Dialogs/Dialogs.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

FSceneTreeViewItem::~FSceneTreeViewItem()
{
}

FSceneTreeViewItem::FSceneTreeViewItem( FArianeObject* InObject, bool bInSensitive )
    : Object( InObject )
    , bSensitive( bInSensitive )
{
}

FArianeObject*
FSceneTreeViewItem::GetObject()
{
    return Object;
}

bool
FSceneTreeViewItem::IsSensitive()
{
    return bSensitive;
}

SArianeEditorSceneTreeViewRow::~SArianeEditorSceneTreeViewRow()
{
}

SArianeEditorSceneTreeViewRow::SArianeEditorSceneTreeViewRow()
    : DropZone( DROPZONE_NONE )
{
}

ECheckBoxState
SArianeEditorSceneTreeViewRow::GetHierarchicalVisibility() const
{
    bool visibility = Item->GetObject()->IsVisible( true );

    return ( visibility ) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

bool
SArianeEditorSceneTreeViewRow::IsVisibilityEnabled() const
{
    FArianeObject* Parent = Item->GetObject()->GetParent();

    return Parent ? Parent->IsVisible( true ) : true;
}

void
SArianeEditorSceneTreeViewRow::OnCheckBoxStateChanged( ECheckBoxState iState )
{
    const TSharedPtr< SArianeEditorSceneTreeView > TreeView = StaticCastSharedPtr<SArianeEditorSceneTreeView>(OwnerTablePtr.Pin());

    // Unregister this widget's updates when the vector scene is updated. We don't want this widget to be
    // rebuilt while it's processing stuff
    TreeView->UnbindComponentDelegates();

    ////GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.object-visibility", "Set Object Visibility"));

    switch( iState )
    {
        case ECheckBoxState::Checked :
            Item->GetObject()->SetVisible( true );
        break;

        case ECheckBoxState::Unchecked :
            Item->GetObject()->SetVisible( false );
        break;

        default :
        break;
    }

    Item->GetObject()->GetImage()->GetDrawingLayer()->GetLayerStack()->GetPainting3DComponent()->Update( false );

    TreeView->BindComponentDelegates();
}

void
SArianeEditorSceneTreeViewRow::Construct( const typename STableRow<TSharedPtr<FSceneTreeViewItem>>::FArguments& InArgs
                                                      , const TSharedRef< STableViewBase >& InOwnerTableView
                                                      , const TSharedPtr<FSceneTreeViewItem> InItem )
{
    Item = InItem;

    SMultiColumnTableRow <TSharedPtr<FSceneTreeViewItem>>::Construct( InArgs, InOwnerTableView );
}

TSharedRef<SWidget>
SArianeEditorSceneTreeViewRow::GenerateWidgetForColumn ( const FName& InColumnName )
{
    FArianeObject* Object = Item->GetObject();

    if( InColumnName == VSTV_OBJECT_VISIBLE )
    {
        const FCheckBoxStyle* isVisibleToggleStyle = &FArianeEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ArianeEditor.SceneTreeView.IsVisibleToggle");

        return SNew(SHorizontalBox)
               + SHorizontalBox::Slot()
               .AutoWidth()
               .HAlign( EHorizontalAlignment::HAlign_Center )
               .VAlign( EVerticalAlignment::VAlign_Center )
               [
                   SNew( SCheckBox )
                  .IsEnabled( Item.Get()->IsSensitive() )
                  .Style( isVisibleToggleStyle )
                  .OnCheckStateChanged( this, &SArianeEditorSceneTreeViewRow::OnCheckBoxStateChanged)
                  .IsChecked( this, &SArianeEditorSceneTreeViewRow::GetHierarchicalVisibility)
                  .IsEnabled( this, &SArianeEditorSceneTreeViewRow::IsVisibilityEnabled )
               ];
    }

    if( InColumnName == VSTV_OBJECT_HUDCOLOR )
    {
        return SNew(SBorder)
               .Padding(3, 2)
               .BorderBackgroundColor( FSlateColor( FLinearColor( 0, 0, 0, 0 ) ) )
               .IsEnabled( Item->GetObject()->HasBaseClass( FArianeGroup::StaticClass() ) )
               .OnMouseButtonDown( FPointerEventHandler::CreateSP( this, &SArianeEditorSceneTreeViewRow::PickColor ) )
               [
                   SNew( SColorBlock )
                  .Color_Lambda( [this]
                                 {
                                     FLinearColor LinearHUDColor = FLinearColor( Item->GetObject()->GetHUDForegroundColor() );

                                     return LinearHUDColor;
                                 } )
               ];
    }

    if( InColumnName == VSTV_OBJECT_NAME )
    {
        const FSlateBrush* ObjectIcon = nullptr;
        FText ObjectIconTooltip = FText::GetEmpty();

        if ( Object->HasBaseClass( FArianeGroup::StaticClass() ) )
        {
            ObjectIcon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.SceneTreeView.Group" );
            ObjectIconTooltip = LOCTEXT("ariane-scene-tree-view.object-icon.group.tooltip", "Group");
        }
        else
        if ( Object->HasBaseClass( FArianePolygon::StaticClass() ) )
        {
            ObjectIcon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.SceneTreeView.Polygon" );
            ObjectIconTooltip = LOCTEXT("ariane-scene-tree-view.object-icon.polygon.tooltip", "Polygon");
        }
        else
        if ( Object->HasBaseClass( FArianeLine::StaticClass() ) )
        {
            ObjectIcon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.SceneTreeView.Line" );
            ObjectIconTooltip = LOCTEXT("ariane-scene-tree-view.object-icon.line.tooltip", "Line");
        }
        else
        if ( Object->HasBaseClass( FArianeRectangle::StaticClass() ) )
        {
            ObjectIcon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.SceneTreeView.Rectangle" );
            ObjectIconTooltip = LOCTEXT("ariane-scene-tree-view.object-icon.rectangle.tooltip", "Rectangle");
        }
        else
        if ( Object->HasBaseClass( FArianeEllipse::StaticClass() ) )
        {
            ObjectIcon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.SceneTreeView.Ellipse" );
            ObjectIconTooltip = LOCTEXT("ariane-scene-tree-view.object-icon.ellipse.tooltip", "Ellipse");
        }
        else
        if ( Object->HasBaseClass( FArianePath::StaticClass() ) )
        {
            ObjectIcon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.SceneTreeView.Path" );
            ObjectIconTooltip = LOCTEXT("ariane-scene-tree-view.object-icon.path.tooltip", "Path");
        }
        else
        {
            ObjectIcon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.SceneTreeView.null16" );
        }

        TextBlockWidget = SNew(SInlineEditableTextBlock)
                          // display cell name only for insensitive objects, i.e objects from another cell
                          .Text( FText::FromName( Object->GetName() ) )
                          .ToolTipText_Lambda( [ Object ]
                                               {
                                                   return FText::FromName( Object->GetName() );
                                               } )
                          .OnVerifyTextChanged( this, &SArianeEditorSceneTreeViewRow::OnVerifyTextChanged )
                          .OnTextCommitted( this, &SArianeEditorSceneTreeViewRow::OnTextChanged );

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
                   .Image( ObjectIcon )
                   .ToolTipText(ObjectIconTooltip)
               ]
               + SHorizontalBox::Slot()
               .Padding( 2, 0 )
               .AutoWidth()
               [
                   TextBlockWidget.ToSharedRef()
               ];
    }

/*
    if( InColumnName == VSTV_OBJECT_TRANSFORMED )
    {
        const FSlateBrush* transformedIcon = nullptr;
        TSharedPtr<SHorizontalBox> tagBox = SNew(SHorizontalBox);

        transformedIcon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.Transform16" );

        return SNew(SHorizontalBox)
               + SHorizontalBox::Slot()
               .AutoWidth()
               .VAlign( EVerticalAlignment::VAlign_Center )
               .HAlign( EHorizontalAlignment::HAlign_Center )
               [
                   SNew( SImage )
                   .Image( transformedIcon )
                   .Visibility_Lambda( [this]
                   {
                       FArianeObject* Object = Item->GetObject();

                       return ( Object->GetLocalMatrix() == BLMatrix2D::make_identity() ) ? EVisibility::Hidden
                                                                                          : EVisibility::Visible;
                   } )
               ];
    }
*/

    if( InColumnName == VSTV_OBJECT_TAGS )
    {
        const FSlateBrush* inbetweenerTagIcon = nullptr;
        TSharedPtr<SHorizontalBox> tagBox = SNew(SHorizontalBox);

        //inbetweenerTagIcon = FOdysseyStyle::GetBrush( "ArianeEditor.SceneTreeView.InbetweenerTag16" );
        inbetweenerTagIcon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.Matching16" );

        for( FArianeTagID& TagID : Object->GetTags() )
        {
            FArianeTag* ObjectTag = TagID.GetTag();
/*
            if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
            {
                //Cells widgets
                tagBox->AddSlot()
                .HAlign( EHorizontalAlignment::HAlign_Center )
                .VAlign( EVerticalAlignment::VAlign_Center )
                [
                    SNew( SImage )
                    .Image( inbetweenerTagIcon )
                ];
            }
*/
        }

        return SNew(SHorizontalBox)
                    .IsEnabled( Item.Get()->IsSensitive() )
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
SArianeEditorSceneTreeViewRow::PickColor( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
    if ( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
    {
        const TSharedPtr< SArianeEditorSceneTreeView > treeView = StaticCastSharedPtr<SArianeEditorSceneTreeView>(OwnerTablePtr.Pin());

        FColorPickerArgs args;

        args.ParentWidget = treeView;
        args.InitialColor = FLinearColor( Item->GetObject()->GetHUDForegroundColor() );
        args.bIsModal = true;
        args.OnColorCommitted = FOnLinearColorValueChanged::CreateSP( this, &SArianeEditorSceneTreeViewRow::OnColorCommitted );

        OpenColorPicker( args );

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

void
SArianeEditorSceneTreeViewRow::OnColorCommitted( FLinearColor Color )
{
    if( Item.Get()->GetObject()->HasBaseClass( FArianeGroup::StaticClass() ) )
    {
        FArianeGroup* Group = static_cast<FArianeGroup*>( Item.Get()->GetObject() );

        Group->UseEditorHUDForegroundColor( false );
        Group->SetHUDForegroundColor( Color.ToFColor( true ) );

        Group->GetPainting3DComponent()->Update( false );
    }
}

FReply
SArianeEditorSceneTreeViewRow::OnMouseButtonUp( const FGeometry & MyGeometry
                                              , const FPointerEvent & MouseEvent )
{
    FArianeGroup* RootGroup = Item.Get()->GetObject()->GetRootGroup();
    FReply reply = FReply::Handled();

    reply = SMultiColumnTableRow::OnMouseButtonUp( MyGeometry, MouseEvent );

    RootGroup->GetPainting3DComponent()->Update( false );

    return reply;
}


FReply
SArianeEditorSceneTreeViewRow::OnMouseButtonDown( const FGeometry & MyGeometry
                                                , const FPointerEvent & MouseEvent )
{
    const TSharedPtr< SArianeEditorSceneTreeView > treeView = StaticCastSharedPtr<SArianeEditorSceneTreeView>(OwnerTablePtr.Pin());
    FArianeObject* Object = Item.Get()->GetObject();

    return SMultiColumnTableRow::OnMouseButtonDown( MyGeometry, MouseEvent );
}

ESelectionMode::Type
SArianeEditorSceneTreeViewRow::GetSelectionMode () const
{
    return Item.Get()->IsSensitive() ? ESelectionMode::Type::Multi :  ESelectionMode::Type::None;
}


bool
SArianeEditorSceneTreeViewRow::OnVerifyTextChanged( const FText& NewText
                                                                , FText& OutErrorMessage )
{
    return true;
}

void
SArianeEditorSceneTreeViewRow::Rename()
{
    TextBlockWidget.Get()->EnterEditingMode();
}

void
SArianeEditorSceneTreeViewRow::OnTextChanged( const FText& InText
                                            , ETextCommit::Type CommitInfo )
{
    const TSharedPtr< SArianeEditorSceneTreeView > TreeView = StaticCastSharedPtr<SArianeEditorSceneTreeView>(OwnerTablePtr.Pin());

    // Unregister this widget's updates when the vector scene is updated. We don't want this widget to be
    // rebuilt while it's processing stuff
    TreeView->UnbindComponentDelegates();

    FArianeObject* ItemObject = Item.Get()->GetObject();
    FArianeGroup* RootGroup = ItemObject->GetRootGroup();

    // needed for valid GUndo pointer
    ////GEditor->BeginTransaction(LOCTEXT("vector-object.transaction.property-changed","Property Changed"));

    Item.Get()->GetObject()->SetName( FName( InText.ToString() ) );

    TextBlockWidget.Get()->SetText( FText::FromName( Item.Get()->GetObject()->GetName() ) );

    RootGroup->GetPainting3DComponent()->Update( false );

    TreeView->BindComponentDelegates();
}

FReply
SArianeEditorSceneTreeViewRow::OnDrop( const FGeometry& iGeometry
                                     , const FDragDropEvent& iDragDropEvent )
{
    const TSharedPtr< SArianeEditorSceneTreeView > TreeView = StaticCastSharedPtr<SArianeEditorSceneTreeView>(OwnerTablePtr.Pin());
    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    //FVector2D position = iGeometry.GetAbsolutePosition();
    FArianeObject* ItemObject = Item.Get()->GetObject();
    FArianeGroup* RootGroup = ItemObject->GetRootGroup();
    TArray<FArianeObject*> SelectedTrees;

    // Unregister this widget's updates when the vector scene is updated. We don't want this widget to be
    // rebuilt while it's processing stuff
    //TreeView->UnbindComponentDelegates();

    RootGroup->GetImage()->GetSelectedTrees( SelectedTrees );

    if( RootGroup->IsSelected() == false )
    {
        GEditor->BeginTransaction(LOCTEXT("ariane-tree-view.transaction.drag-drop-object", "Drop Objects"));

        RootGroup->GetImage()->GetDrawingLayer()->Modify();

        for( FArianeObject* SelectedTree : SelectedTrees )
        {
            switch( DropZone )
            {
                //case DROPZONE_ABOVE:
                // reverse order in order to get the most forward objet on top of the hierarchy
                case DROPZONE_BELOW:
                {
                    FArianeObject* ParentObject = ItemObject->GetParent() ? ItemObject->GetParent() : ItemObject;

                    // note: SharedEnv and Root are system objects
                    //if( parentObject->IsSystem() == false )
                    {
                        // don't drop onto the same object or else expect some infinite loop
                        if( ParentObject != SelectedTree )
                        {
                            ParentObject->TransferChild( SelectedTree, ParentObject->GetPreviousChild( ItemObject ) );
                        }
                    }
                }
                break;

                case DROPZONE_ONTO:
                    // don't drop onto the same object or else expect some infinite loop
                    if( ItemObject != SelectedTree )
                    {
                        ItemObject->TransferChild( SelectedTree, nullptr );
                    }
                break;

                // case DROPZONE_BELOW:
                // reverse order in order to get the most forward objet on top of the hierarchy
                case DROPZONE_ABOVE:
                {
                    FArianeObject* ParentObject = ItemObject->GetParent() ? ItemObject->GetParent() : ItemObject;

                    if( ParentObject != SelectedTree )
                    {
                        ParentObject->TransferChild( SelectedTree, ItemObject );
                    }
                }
                break;

                default :
                break;
            }
        }

        DropZone = DROPZONE_NONE;

        RootGroup->GetPainting3DComponent()->Update( false );

        GEditor->EndTransaction();

        //TreeView->BindComponentDelegates();
    }

    return FReply::Handled();
}

int32
SArianeEditorSceneTreeViewRow::OnPaint( const FPaintArgs& Args
                                                    , const FGeometry& AllottedGeometry
                                                    , const FSlateRect& MyCullingRect
                                                    , FSlateWindowElementList& OutDrawElements
                                                    , int32 LayerId
                                                    , const FWidgetStyle& InWidgetStyle
                                                    , bool bParentEnabled ) const
{
    const FTableRowStyle& style = FArianeEditorStyle::Get().GetWidgetStyle<FTableRowStyle>("ArianeEditor.LayerStack.AlternatedRows");
    const FSlateBrush* DropIndicatorBrush = nullptr;

    int32 rowLayerId = SMultiColumnTableRow<TSharedPtr<FSceneTreeViewItem>>::OnPaint( Args
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
SArianeEditorSceneTreeViewRow::OnDragEnter( const FGeometry& MyGeometry
                                          , const FDragDropEvent& DragDropEvent )
{
    DropZone = DROPZONE_ONTO;
}

void
SArianeEditorSceneTreeViewRow::OnDragLeave( const FDragDropEvent& DragDropEvent )
{
    DropZone = DROPZONE_NONE;
}

FReply
SArianeEditorSceneTreeViewRow::OnDragOver( const FGeometry& iGeometry
                                         , const FDragDropEvent& iDragDropEvent )
{
    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    //FVector2D position = iGeometry.GetAbsolutePosition();
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
SArianeEditorSceneTreeViewRow::OnDragDetected ( const FGeometry& iGeometry
                                              , const FPointerEvent& iMouseEvent )
{
    TSharedRef<FDragDropOperation> Operation = MakeShared<FDragDropOperation>();

    return FReply::Handled().BeginDragDrop(Operation);
}

#undef LOCTEXT_NAMESPACE
