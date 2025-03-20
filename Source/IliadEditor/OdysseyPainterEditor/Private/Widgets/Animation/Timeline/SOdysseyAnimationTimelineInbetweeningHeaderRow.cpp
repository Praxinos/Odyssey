// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineInbetweeningHeader.h"

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "OdysseyLayerStack.h"

#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "OdysseyStyle.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
// from module OdysseyVector
#include "OdysseyVector.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorGroupPaint.h"
#include "Undo/OdysseyVectorUndoObjectParam.h"

#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FInbetweeningListViewItem::~FInbetweeningListViewItem()
{
}

FInbetweeningListViewItem::FInbetweeningListViewItem( FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    mInbetweenerTag = iInbetweenerTag;
}

FOdysseyVectorTagInbetweener*
FInbetweeningListViewItem::GetInbetweenerTag()
{
    return mInbetweenerTag;
}

SOdysseyAnimationTimelineInbetweeningHeaderRow::~SOdysseyAnimationTimelineInbetweeningHeaderRow()
{
}

SOdysseyAnimationTimelineInbetweeningHeaderRow::SOdysseyAnimationTimelineInbetweeningHeaderRow()
{
}

ECheckBoxState
SOdysseyAnimationTimelineInbetweeningHeaderRow::GetVisibility() const
{
    bool visibility = mInbetweenerTag->GetOwner()->IsVisible( true );

    return ( visibility ) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

bool
SOdysseyAnimationTimelineInbetweeningHeaderRow::IsVisibilityEnabled() const
{
    return mInbetweenerTag->GetOwner()->GetParent()->IsVisible( true );
}

void
SOdysseyAnimationTimelineInbetweeningHeaderRow::OnCheckBoxStateChanged( ECheckBoxState iState )
{
    uint32 notificationFlags = 0;

    GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.object-visibility", "Set Object Visibility"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectParam( mInbetweenerTag->GetOwner()->GetScene()
                                                                                                      , mInbetweenerTag->GetOwner()
                                                                                                      , notificationFlags ) );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        const TSharedPtr< SOdysseyAnimationTimelineInbetweeningHeader > treeView = StaticCastSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader>(OwnerTablePtr.Pin());
        TSharedPtr<FOdysseyPainterEditorSource> source = treeView->GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    switch( iState )
    {
        case ECheckBoxState::Checked :
            mInbetweenerTag->GetOwner()->SetVisible( true );
        break;

        case ECheckBoxState::Unchecked :
            mInbetweenerTag->GetOwner()->SetVisible( false );
        break;

        default :
        break;
    }

    mInbetweenerTag->GetOwner()->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    mInbetweenerTag->GetOwner()->GetLayer()->RequestRedraw( mInbetweenerTag->GetOwner()->GetCell(), 0 );
}

void
SOdysseyAnimationTimelineInbetweeningHeaderRow::Construct( const typename STableRow<TSharedPtr<FInbetweeningListViewItem>>::FArguments& InArgs
                                                         , const TSharedRef< STableViewBase >& InOwnerTableView
                                                         , const TSharedPtr<FInbetweeningListViewItem> iTem )
{
    static FTableRowStyle style = FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("Inbetweening.TableRow");
    const FCheckBoxStyle* isActivatedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("LayerStack.IsActivatedToggle");
    STableRow<TSharedPtr<FInbetweeningListViewItem>>::Construct( InArgs, InOwnerTableView );
    TSharedPtr<SHorizontalBox> tagBox;
    const FSlateBrush* objectIcon = nullptr;
    const FSlateBrush* inbetweenerTagIcon = FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Matching16" );
    FOdysseyVectorTagInbetweener* inbetweenerTag = iTem.Get()->GetInbetweenerTag();
    uint32 cellIndex = inbetweenerTag->GetOwner()->GetCell()->GetIndex();

    mInbetweenerTag = inbetweenerTag;

    mTextBlockWidget = SNew(STextBlock)
                       .Text( FText::FromString( FString::Printf( TEXT("Cell %d / "), cellIndex )
                                               + mInbetweenerTag->GetOwner()->GetName() ) );

    tagBox = SNew(SHorizontalBox);

    SetContent( SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .VAlign(EVerticalAlignment::VAlign_Center)
                .Padding( 2, 0 )
                .AutoWidth()
                [
                    SNew( SImage )
                    .Image( inbetweenerTagIcon )
                ]
                + SHorizontalBox::Slot()
                .VAlign(EVerticalAlignment::VAlign_Center)
                .Padding( 2, 0 )
                .AutoWidth()
                [
                    SNew( SCheckBox )
                    .Style(isActivatedToggleStyle)
                    .OnCheckStateChanged(this, &SOdysseyAnimationTimelineInbetweeningHeaderRow::OnCheckBoxStateChanged)
                    .IsChecked(this, &SOdysseyAnimationTimelineInbetweeningHeaderRow::GetVisibility)
                    .IsEnabled( this, &SOdysseyAnimationTimelineInbetweeningHeaderRow::IsVisibilityEnabled )
                ]
                + SHorizontalBox::Slot()
                .VAlign(EVerticalAlignment::VAlign_Center)
                .Padding( 2, 0 )
                .AutoWidth()
                [
                    mTextBlockWidget.ToSharedRef()
                ] );

    Style = &style;
}

int32
SOdysseyAnimationTimelineInbetweeningHeaderRow::OnPaint( const FPaintArgs& Args
                                                       , const FGeometry& AllottedGeometry
                                                       , const FSlateRect& MyCullingRect
                                                       , FSlateWindowElementList& OutDrawElements
                                                       , int32 LayerId
                                                       , const FWidgetStyle& InWidgetStyle
                                                       , bool bParentEnabled ) const
{
    TSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader> treeView = StaticCastSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader>(OwnerTablePtr.Pin());
    UOdysseyAnimationLayerImageVector* layer = treeView.Get()->GetAnimationLayerImageVector();
    UOdysseyLayerStack* layerStack = layer->GetLayerStack();
    static FSlateBrush defaultBrush;

    if( layerStack->CurrentLayer == layer )
    {
        if( mInbetweenerTag->GetOwner()->IsSelected() )
        {
            FSlateDrawElement::MakeBox( OutDrawElements
                                      , LayerId
                                      , AllottedGeometry.ToPaintGeometry()
                                      , &defaultBrush
                                      , ESlateDrawEffect::None
                                      , FStyleColors::PrimaryPress.GetColor( FWidgetStyle() ) );
        }
    }

    LayerId = STableRow<TSharedPtr<FInbetweeningListViewItem>>::OnPaint( Args
                                                                       , AllottedGeometry
                                                                       , MyCullingRect
                                                                       , OutDrawElements
                                                                       , LayerId
                                                                       , InWidgetStyle
                                                                       , bParentEnabled );

    return ++LayerId;
}

FReply
SOdysseyAnimationTimelineInbetweeningHeaderRow::OnMouseButtonDown( const FGeometry & MyGeometry
                                                                 , const FPointerEvent & MouseEvent )
{
    TSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader> treeView = StaticCastSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader>(OwnerTablePtr.Pin());
    UOdysseyAnimationLayerImageVector* layer = treeView.Get()->GetAnimationLayerImageVector();
    FOdysseyPainterEditor* editor = treeView->GetEditor();
    UOdysseyLayerStack* layerStack = editor->LayerStack();

    if ( layerStack->CurrentLayer.Get() != layer )
    {
        FOdysseyObjectEditorUtils::SetPropertyValue( layerStack, "CurrentLayer", layer );
    }

    return STableRow<TSharedPtr<FInbetweeningListViewItem>>::OnMouseButtonDown( MyGeometry, MouseEvent );
}

bool
SOdysseyAnimationTimelineInbetweeningHeaderRow::IsItemSelected() const
{
    return mInbetweenerTag->GetOwner()->IsSelected();
}

FVector2D
SOdysseyAnimationTimelineInbetweeningHeaderRow::ComputeDesiredSize ( float LayoutScaleMultiplier ) const
{
    return FVector2D( 64
                    , UOdysseyAnimationLayerImageVector::GetInbetweeningRowHeight() );
}

#undef LOCTEXT_NAMESPACE
