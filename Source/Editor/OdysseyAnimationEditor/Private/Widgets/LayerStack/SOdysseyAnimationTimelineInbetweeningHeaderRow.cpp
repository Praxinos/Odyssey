// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    uint32 cellIndex = inbetweenerTag->GetOwner()->GetEngine()->GetAnimationCell()->GetIndex();

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
    FOdysseyAnimationEditorExtension* animationEditorExtension = treeView.Get()->GetAnimationEditorExtension();
    UOdysseyAnimationLayerImageVector* layer = treeView.Get()->GetAnimationLayerImageVector();
    UOdysseyLayerStack* layerStack = animationEditorExtension->GetEditor()->LayerStack();
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
    return FVector2D( 20.0f * LayoutScaleMultiplier, 20.0f * LayoutScaleMultiplier );
}

#undef LOCTEXT_NAMESPACE
