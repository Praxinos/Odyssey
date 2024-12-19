// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeader.h"

#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "OdysseyLayerStack.h"

#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "OdysseyStyleSet.h"
#include "OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
// from module OdysseyVector
#include "OdysseyVector.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorGroupPaint.h"
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

void
SOdysseyAnimationTimelineInbetweeningHeaderRow::Construct( const typename STableRow<TSharedPtr<FInbetweeningListViewItem>>::FArguments& InArgs
                                                         , const TSharedRef< STableViewBase >& InOwnerTableView
                                                         , const TSharedPtr<FInbetweeningListViewItem> iTem )
{
    STableRow<TSharedPtr<FInbetweeningListViewItem>>::Construct( InArgs, InOwnerTableView );
    TSharedPtr<SHorizontalBox> tagBox;
    const FSlateBrush* objectIcon = nullptr;
    const FSlateBrush* inbetweenerTagIcon = FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Matching16" );
    FOdysseyVectorTagInbetweener* inbetweenerTag = iTem.Get()->GetInbetweenerTag();
    uint32 cellIndex = inbetweenerTag->GetOwner()->GetEngine()->GetCell()->GetIndex();

    mInbetweenerTag = inbetweenerTag;

    mTextBlockWidget = SNew(STextBlock)
                       .Text( FText::FromString( FString::Printf( TEXT("Cell %d / "), cellIndex )
                                               + mInbetweenerTag->GetOwner()->GetName() ) );

    tagBox = SNew(SHorizontalBox);

    SetContent( SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SImage )
                    .Image( inbetweenerTagIcon )
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    mTextBlockWidget.ToSharedRef()
                ] );
}

FReply
SOdysseyAnimationTimelineInbetweeningHeaderRow::OnMouseButtonUp( const FGeometry & MyGeometry
                                                               , const FPointerEvent & MouseEvent )
{
    return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineInbetweeningHeaderRow::OnMouseButtonDown( const FGeometry & MyGeometry
                                                                 , const FPointerEvent & MouseEvent )
{
    TSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader> treeView = StaticCastSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader>(OwnerTablePtr.Pin());
    UOdysseyAnimationLayerImageVector* layer = treeView.Get()->GetAnimationLayerImageVector();
    FOdysseyPainterEditor* editor = treeView->GetEditor();
    UOdysseyLayerStack* layerStack = editor->LayerStack();
    FOdysseyVectorObject* ownerObject = mInbetweenerTag->GetOwner();
    FOdysseyVectorEngine* vectorEngine = ownerObject->GetEngine();
    FOdysseyVectorSharedEnv* sharedEnv = mInbetweenerTag->GetOwner()->GetSharedEnv();
    std::list<FOdysseyVectorTag*>& sharedTagList = sharedEnv->GetSharedTagList();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;
    FReply reply = FReply::Unhandled();

    if ( layerStack->CurrentLayer.Get() != layer )
    {
        FOdysseyObjectEditorUtils::SetPropertyValue( layerStack
                                                    , "CurrentLayer"
                                                    , TSoftObjectPtr<UOdysseyLayer>( layer ) );
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
        reply = FReply::Handled();
    }

    // request redraw
    mInbetweenerTag->GetOwner()->GetScene()->GetEngine()->Invalidate( 0 );
    // update UI
    FOdysseyVectorEngine::Notify( nullptr, notificationFlags );

    return reply;
}

bool
SOdysseyAnimationTimelineInbetweeningHeaderRow::IsItemSelected() const
{
    return mInbetweenerTag->GetOwner()->IsSelected();
}

FVector2D
SOdysseyAnimationTimelineInbetweeningHeaderRow::ComputeDesiredSize ( float LayoutScaleMultiplier ) const
{
    return FVector2D( 20.0f /* * LayoutScaleMultiplier*/, 20.0f /* * LayoutScaleMultiplier*/ );
}

#undef LOCTEXT_NAMESPACE
